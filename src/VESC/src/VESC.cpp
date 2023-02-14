//
// Created by andyh on 2/14/23.
//

#include "VESC.h"

#include "CANMessages.h"

// STL
#include <utility>
#include <iostream>

using namespace evgp_project::vesc;

VESC::VESC(std::shared_ptr<CANBus> canbus, unsigned int canID)
    : _canbus(std::move(canbus)),
      _canID(canID),
      _pingPongThreadRunning(true),
      _pingCount(0),
      _pongCount(0)
{
    _canbus->subscribeFrameID(0x80001200, [this](const can_frame&)
    {
        _pongCount++;
    });

    _pingPongThread = std::thread([this](){ pingPong(); });
}

VESC::~VESC()
{
    _pingPongThreadRunning = false;

    if(_pingPongThread.joinable())
        _pingPongThread.join();
}

void VESC::pingPong()
{
    while(_pingPongThreadRunning)
    {
        // Send ping
        std::vector<uint8_t> payload = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, static_cast<uint8_t>((_canID & 0xF0) >> 4), static_cast<uint8_t>(_canID & 0xF)};
        _canbus->sendFrame(buildCANExtendedID(evgp_cart::vesc::CANMessages::Ping, _canID), payload);
        _pingCount++;

        // Motor
//        std::vector<uint8_t> payload2 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x98, 0x96, 0x80 };
//        std::vector<uint8_t> payload2 = {0x80, 0x96, 0x98, 0x0, 0x0, 0x0, 0x0, 0x0 };
        std::vector<uint8_t> payload2 = {0x5F, 0x5E, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0 };
        _canbus->sendFrame(buildCANExtendedID(evgp_cart::vesc::CANMessages::SetDutyPercent, _canID), payload2);

        // Delay
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}
