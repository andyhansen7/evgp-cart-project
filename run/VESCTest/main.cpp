//
// Created by andyh on 2/14/23.
//

#include <CAN/src/CANBus.h>
#include <VESC/src/VESC.h>
#include <VESC/src/CANUtils.h>
#include <VESC/src/CANMessages.h>

#include <memory>
#include <atomic>
#include <csignal>
#include <iostream>

using namespace evgp_project;

std::atomic<bool> interrupted = false;

void signal(int)
{
    interrupted = true;
}

int main()
{
//    const auto bus = std::make_shared<CANBus>("can0");
//    const auto vesc = std::make_shared<vesc::VESC>(bus, 48);
//
//    while(!interrupted)
//    {
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        std::cout << "Sent " << vesc->getPings() << ", received " << vesc->getPongs() << std::endl;
//    }

    std::cout << std::hex << vesc::buildCANExtendedID(vesc::CANMessages::Pong, 0) << std::endl;
    std::cout << "0x80001200" << std::endl;

    return 0;
}
