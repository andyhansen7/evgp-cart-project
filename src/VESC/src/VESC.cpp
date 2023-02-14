//
// Created by andyh on 2/14/23.
//

#include "VESC.h"

#include "CANMessages.h"

// STL
#include <utility>

using namespace evgp_project::vesc;

VESC::VESC(std::shared_ptr<CANBus> canbus, unsigned int canID)
    : _canbus(std::move(canbus)),
      _canID(canID),
      _pingPongThreadRunning(true)
{
//    _canbus->subscribeFrameID();

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


        _canbus->sendFrame(evgp_cart::vesc::CANMessages::Ping)
    }
}
