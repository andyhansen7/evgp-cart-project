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
        VESC(std::shared_ptr<CANBus> canbus,
             uint8_t canID);

        ~VESC();

        // Helper functions for getting the total number of pings sent to the controller, and the number of pongs it sent back
        unsigned long getPings() const;
        unsigned long getPongs() const;

        // Getter methods for class members
        float getRPM() const;
        float getOutputCurrent() const;
        float getDutyCycle() const;
        float getTotalConsumedAmpHours() const;
        float getTotalAmpHoursRecharged() const;
        float getTotalConsumedWattHours() const;
        float getTotalWattHoursRecharged() const;
        float getMosfetTemperature() const;
        float getMotorTemperature() const;
        float getTotalFilteredMotorCurrent() const;
        float getPIDPosition() const;
        float getTachometerValue() const;
        float getInputVoltage() const;

    private:
        std::shared_ptr<CANBus> _canbus;
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
