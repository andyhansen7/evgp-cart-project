//
// Created by andyh on 2/14/23.
//

#ifndef EVGP_CART_VESC_H
#define EVGP_CART_VESC_H

// CAN
#include <CAN/src/CANBus.h>

// STL
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>

namespace evgp_project::vesc
{
    class VESC
    {
    public:
        VESC(std::shared_ptr<can::CANBus> canbus,
             uint8_t canID);

        ~VESC();

        // Set the speed of the motor as PWM [-1, 1]
        [[maybe_unused]] bool setPWM(float pwm) const;

        // Set the speed of the motor in rpm
        [[maybe_unused]] bool setRPM(float rpm) const;

        // Set the current brake in amps
        [[maybe_unused]] bool setCurrentBrake(float amps) const;

        // Helper functions for getting the total number of pings sent to the controller, and the number of pongs it sent back
        [[maybe_unused]] unsigned long getPings() const;
        [[maybe_unused]] unsigned long getPongs() const;

        // Getter methods for class members
        [[maybe_unused]] float getRPM() const;
        [[maybe_unused]] float getOutputCurrent() const;
        [[maybe_unused]] float getDutyCycle() const;
        [[maybe_unused]] float getTotalConsumedAmpHours() const;
        [[maybe_unused]] float getTotalAmpHoursRecharged() const;
        [[maybe_unused]] float getTotalConsumedWattHours() const;
        [[maybe_unused]] float getTotalWattHoursRecharged() const;
        [[maybe_unused]] float getMosfetTemperature() const;
        [[maybe_unused]] float getMotorTemperature() const;
        [[maybe_unused]] float getTotalFilteredMotorCurrent() const;
        [[maybe_unused]] float getPIDPosition() const;
        [[maybe_unused]] float getTachometerValue() const;
        [[maybe_unused]] float getInputVoltage() const;

    private:
        std::shared_ptr<can::CANBus> _canbus;
        const uint8_t _canID;

        // Ping-pong
        std::atomic<bool> _pingPongThreadRunning;
        std::thread _pingPongThread;
        std::atomic<unsigned long> _pingCount;
        std::atomic<unsigned long> _pongCount;

        // Status frame 1 members
        mutable std::mutex _statusFrame1Mutex;
        float _rpm;
        float _current_A;
        float _dutyCycle_percent;

        // Status frame 2 members
        mutable std::mutex _statusFrame2Mutex;
        float _totalConsumedAmpHours;
        float _totalRechargedAmpHours;

        // Status frame 3 members
        mutable std::mutex _statusFrame3Mutex;
        float _totalConsumedWattHours;
        float _totalRechargedWattHours;

        // Status frame 4 members
        mutable std::mutex _statusFrame4Mutex;
        float _mosfetTemperature;
        float _motorTemperature;
        float _totalFilteredMotorCurrentA;  // TODO: what is this???
        float _pidPosition;                 // And this

        // Status frame 5 members
        mutable std::mutex _statusFrame5Mutex;
        float _tachometerValue;
        float _inputVoltage;

        // Helper to run ping pong to the controller
        void pingPong();
    };
}


#endif //EVGP_CART_VESC_H
