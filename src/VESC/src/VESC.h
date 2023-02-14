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
             unsigned canID);

        ~VESC();

    private:
        std::shared_ptr<CANBus> _canbus;
        const unsigned _canID;

        std::atomic<bool> _pingPongThreadRunning;
        std::thread _pingPongThread;

        // Helper to run ping pong to the controller
        void pingPong();

        // Helper for formatting data
        static inline std::vector<uint8_t> unsigned4Bytes(const unsigned data)
        {
            std::vector<uint8_t> vec;
            vec.resize(4);

            vec[3] = (data >> 24) & 0xFF;
            vec[2] = (data >> 16) & 0xFF;
            vec[1] = (data >> 8) & 0xFF;
            vec[0] = (data >> 0) & 0xFF;

            return vec;
        }

        // Helper to build a CAN frame ID
        static inline uint32_t buildCANExtendedID(const uint32_t frameID, const uint32_t deviceID)
        {
            return (frameID | deviceID | CAN_EFF_FLAG);
        }
    };
}


#endif //EVGP_CART_VESC_H
