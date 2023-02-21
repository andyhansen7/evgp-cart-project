//
// Created by andyh on 2/14/23.
//

#include "VESC.h"

#include "CANMessages.h"
#include "CANUtils.h"

// STL
#include <utility>
#include <iostream>

using namespace evgp_project::vesc;


VESC::VESC(std::shared_ptr<can::CANBus> canbus, const uint8_t canID)
    : _canbus(std::move(canbus)),
      _canID(canID),
      _transmitThreadRunning(true),
      _pingCount(0),
      _pongCount(0),
      _rpmTarget(0.0f),
      _coast(true),
      _rpm(0.0f),
      _current_A(0.0f),
      _dutyCycle_percent(0.0f),
      _totalConsumedAmpHours(0.0f),
      _totalRechargedAmpHours(0.0f),
      _totalConsumedWattHours(0.0f),
      _totalRechargedWattHours(0.0f),
      _mosfetTemperature(0.0f),
      _motorTemperature(0.0f),
      _totalFilteredMotorCurrentA(0.0f),
      _pidPosition(0.0f),
      _tachometerValue(0.0f),
      _inputVoltage(0.0f)
{
    // Subscribe to pongs from controller
    _canbus->subscribeFrameID(vesc::buildCANExtendedID(vesc::CANMessages::Pong, 0), [this](const can_frame&)
    {
        _pongCount++;
    });

    // Subscribe to status frames
    _canbus->subscribeFrameID(vesc::buildCANExtendedID(vesc::CANMessages::Status1, _canID), [this](const can_frame& frame)
    {
        uint32_t rawRPM = 0;
        uint32_t rawCurrent = 0;
        uint32_t rawDutyCycle = 0;

        rawRPM |= frame.data[0] << 24;
        rawRPM |= frame.data[1] << 16;
        rawRPM |= frame.data[2] << 8;
        rawRPM |= frame.data[3];

        rawCurrent |= frame.data[4] << 8;
        rawCurrent |= frame.data[5];

        rawDutyCycle |= frame.data[6] << 8;
        rawDutyCycle |= frame.data[7];

        auto rpm = static_cast<float>(rawRPM);
        float current = static_cast<float>(rawCurrent) / static_cast<float>(1e1);
        float dutyCycle = static_cast<float>(rawDutyCycle) / static_cast<float>(1e3);

        std::scoped_lock lock(_statusFrame1Mutex);
        _rpm = rpm;
        _current_A = current;
        _dutyCycle_percent = dutyCycle;
    });

    _canbus->subscribeFrameID(vesc::buildCANExtendedID(vesc::CANMessages::Status2, _canID), [this](const can_frame& frame)
    {
        uint32_t rawAmpHours = 0;
        uint32_t rawAmpHoursCharged = 0;

        rawAmpHours |= frame.data[0] << 24;
        rawAmpHours |= frame.data[1] << 16;
        rawAmpHours |= frame.data[2] << 8;
        rawAmpHours |= frame.data[3];

        rawAmpHoursCharged |= frame.data[4] << 24;
        rawAmpHoursCharged |= frame.data[5] << 16;
        rawAmpHoursCharged |= frame.data[6] << 8;
        rawAmpHoursCharged |= frame.data[7];

        float ampHours = static_cast<float>(rawAmpHours) / static_cast<float>(1e4);
        float ampHoursCharged = static_cast<float>(rawAmpHoursCharged) / static_cast<float>(1e4);

        std::scoped_lock lock(_statusFrame2Mutex);
        _totalConsumedAmpHours = ampHours;
        _totalRechargedAmpHours = ampHoursCharged;
    });

    _canbus->subscribeFrameID(vesc::buildCANExtendedID(vesc::CANMessages::Status3, _canID), [this](const can_frame& frame)
    {
        uint32_t rawConsumedWattHours = 0;
        uint32_t rawChargedWattHours = 0;

        rawConsumedWattHours |= frame.data[0] << 24;
        rawConsumedWattHours |= frame.data[1] << 16;
        rawConsumedWattHours |= frame.data[2] << 8;
        rawConsumedWattHours |= frame.data[3];

        rawChargedWattHours |= frame.data[4] << 24;
        rawChargedWattHours |= frame.data[5] << 16;
        rawChargedWattHours |= frame.data[6] << 8;
        rawChargedWattHours |= frame.data[7];

        float wattHours = static_cast<float>(rawConsumedWattHours) / static_cast<float>(1e4);
        float wattHoursCharged = static_cast<float>(rawChargedWattHours) / static_cast<float>(1e4);

        std::scoped_lock lock(_statusFrame3Mutex);
        _totalConsumedWattHours = wattHours;
        _totalRechargedWattHours = wattHoursCharged;
    });

    _canbus->subscribeFrameID(vesc::buildCANExtendedID(vesc::CANMessages::Status4, _canID), [this](const can_frame& frame)
    {
        uint32_t rawMosfetTemperature = 0;
        uint32_t rawMotorTemperature = 0;
        uint32_t rawFilteredCurrent = 0;
        uint32_t rawPIDPosition = 0;

        rawMosfetTemperature |= frame.data[0] << 8;
        rawMosfetTemperature |= frame.data[1];

        rawMotorTemperature |= frame.data[2] << 8;
        rawMotorTemperature |= frame.data[3];

        rawFilteredCurrent |= frame.data[4] << 8;
        rawFilteredCurrent |= frame.data[5];

        rawPIDPosition |= frame.data[6] << 8;
        rawPIDPosition |= frame.data[7];

        float mosfetTemperature = static_cast<float>(rawMosfetTemperature) / static_cast<float>(1e1);
        float motorTemperature = static_cast<float>(rawMotorTemperature) / static_cast<float>(1e1);
        float filteredCurrent = static_cast<float>(rawFilteredCurrent) / static_cast<float>(1e1);
        float pidPosition = static_cast<float>(rawPIDPosition) / static_cast<float>(50);


        std::scoped_lock lock(_statusFrame4Mutex);
        _mosfetTemperature = mosfetTemperature;
        _motorTemperature = motorTemperature;
        _totalFilteredMotorCurrentA = filteredCurrent;
        _pidPosition = pidPosition;
    });

    _canbus->subscribeFrameID(vesc::buildCANExtendedID(vesc::CANMessages::Status5, _canID), [this](const can_frame& frame)
    {
        uint32_t rawTachometerValue = 0;
        uint32_t rawInputvoltage = 0;

        rawTachometerValue |= frame.data[0] << 24;
        rawTachometerValue |= frame.data[1] << 16;
        rawTachometerValue |= frame.data[2] << 8;
        rawTachometerValue |= frame.data[3];

        rawInputvoltage |= frame.data[4] << 8;
        rawInputvoltage |= frame.data[5];

        auto tachometerValue = static_cast<float>(rawTachometerValue);
        float inputVoltage = static_cast<float>(rawInputvoltage) / static_cast<float>(1e1);

        std::scoped_lock lock(_statusFrame5Mutex);
        _tachometerValue = tachometerValue;
        _inputVoltage = inputVoltage;
    });

    _transmitThread = std::thread([this](){ transmitMessages(); });
}

VESC::~VESC()
{
    _transmitThreadRunning = false;

    if(_transmitThread.joinable())
        _transmitThread.join();
}

void VESC::setRPM(const float rpm)
{
   _rpmTarget = rpm;
   _coast = false;
}

void VESC::setCoast(const bool coast)
{
    _coast = coast;
}

void VESC::setCurrentBrake(float amps) const
{
    // TODO move to transmit
    auto data = static_cast<uint32_t>(amps * 1000.0f);
    std::vector<uint8_t> payload;
    buildCANFramePayload(payload, 0, 0, 0, 0, byte0(data), byte1(data), byte2(data), byte3(data));

    _canbus->sendFrame(vesc::buildCANExtendedID(vesc::CANMessages::SetCurrentBrakeA, _canID), payload);
}

unsigned long VESC::getPings() const
{
    return _pingCount;
}

unsigned long VESC::getPongs() const
{
    return _pongCount;
}

float VESC::getRPM() const
{
    std::scoped_lock lock(_statusFrame1Mutex);
    return _rpm;
}

float VESC::getOutputCurrent() const
{
    std::scoped_lock lock(_statusFrame1Mutex);
    return _current_A;
}

float VESC::getDutyCycle() const
{
    std::scoped_lock lock(_statusFrame1Mutex);
    return _dutyCycle_percent;
}

float VESC::getTotalConsumedAmpHours() const
{
    std::scoped_lock lock(_statusFrame2Mutex);
    return _totalConsumedAmpHours;
}

float VESC::getTotalAmpHoursRecharged() const
{
    std::scoped_lock lock(_statusFrame2Mutex);
    return _totalRechargedAmpHours;
}

float VESC::getTotalConsumedWattHours() const
{
    std::scoped_lock lock(_statusFrame3Mutex);
    return _totalConsumedWattHours;
}

float VESC::getTotalWattHoursRecharged() const
{
    std::scoped_lock lock(_statusFrame3Mutex);
    return _totalRechargedWattHours;
}

float VESC::getMosfetTemperature() const
{
    std::scoped_lock lock(_statusFrame4Mutex);
    return _mosfetTemperature;
}

float VESC::getMotorTemperature() const
{
    std::scoped_lock lock(_statusFrame4Mutex);
    return _motorTemperature;
}

float VESC::getTotalFilteredMotorCurrent() const
{
    std::scoped_lock lock(_statusFrame4Mutex);
    return _totalFilteredMotorCurrentA;
}

float VESC::getPIDPosition() const
{
    std::scoped_lock lock(_statusFrame4Mutex);
    return _pidPosition;
}

float VESC::getTachometerValue() const
{
    std::scoped_lock lock(_statusFrame5Mutex);
    return _tachometerValue;
}

float VESC::getInputVoltage() const
{
    std::scoped_lock lock(_statusFrame5Mutex);
    return _inputVoltage;
}

void VESC::transmitMessages()
{
    std::vector<uint8_t> payload;
    buildCANFramePayload(payload, 0, 0, 0, 0, 0, 0, 0, _canID);

    while(_transmitThreadRunning)
    {
        // Send ping
        _canbus->sendFrame(buildCANExtendedID(CANMessages::Ping, _canID), payload);
        _pingCount++;

        // Set RPM
        const bool inCoast = _coast;
        const bool useRPMSet = std::abs(_rpmTarget) > _usePWMInsteadOfRPMThreshold;

        if(!inCoast)
        {
            if(useRPMSet)
            {
                auto data = static_cast<int>(_rpmTarget);

                buildCANFramePayload(payload, 0, 0, 0, 0, byte0((unsigned)data), byte1((unsigned)data), byte2((unsigned)data), byte3((unsigned)data));

                _canbus->sendFrame(vesc::buildCANExtendedID(vesc::CANMessages::SetRPM, _canID), payload);
            }
            else
            {
                float pwmTarget = _rpmTarget * _rpmToPWMFactor;

                if(pwmTarget < -1.0f) pwmTarget = -1.0f;
                if(pwmTarget > 1.0f) pwmTarget = 1.0f;

                auto data = static_cast<int>(pwmTarget * 100000.0);

                buildCANFramePayload(payload, 0, 0, 0, 0, byte0((unsigned)data), byte1((unsigned)data), byte2((unsigned)data), byte3((unsigned)data));

                _canbus->sendFrame(vesc::buildCANExtendedID(vesc::CANMessages::SetDutyPercent, _canID), payload);
            }
        }

        // Delay
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
