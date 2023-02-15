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

VESC::VESC(std::shared_ptr<CANBus> canbus, const uint8_t canID)
    : _canbus(std::move(canbus)),
      _canID(canID),
      _pingPongThreadRunning(true),
      _pingCount(0),
      _pongCount(0),
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

    });

    _pingPongThread = std::thread([this](){ pingPong(); });
}

VESC::~VESC()
{
    _pingPongThreadRunning = false;

    if(_pingPongThread.joinable())
        _pingPongThread.join();
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
    return _totalRechargedWattHours;
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

void VESC::pingPong()
{
    while(_pingPongThreadRunning)
    {
        // Send ping
        _canbus->sendFrame(buildCANExtendedID(CANMessages::Ping, _canID),
                           buildCANFramePayload(0, 0, 0, 0, 0, 0, 0, _canID));
        _pingCount++;

        // Delay
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}
