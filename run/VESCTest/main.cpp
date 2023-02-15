//
// Created by andyh on 2/14/23.
//

#include <CAN/src/CANBus.h>
#include <VESC/src/VESC.h>
#include <GUI/src/SimpleGUI.h>

// STL
#include <memory>
#include <atomic>
#include <csignal>
#include <iostream>

using namespace evgp_project;

std::atomic<bool> interrupted = false;

void handler(int)
{
    interrupted = true;
}

int main()
{
    signal(SIGINT, handler);

    static constexpr unsigned vescID = 48;

//    const auto bus = std::make_shared<can::CANBus>("can0");
//    const auto vesc = std::make_shared<vesc::VESC>(bus, vescID);
//
    gui::GUIEntry rpm = {
        .name = "RPM",
        .precision = 6,
        .value = []() {
            return 1.0f;
        }
    };

    gui::GUIEntry current = {
            .name = "Current (A)",
            .precision = 6,
            .value = []() {
                return 2.0f;
            }
    };

    gui::KeybindEntry eStop = {
        .key = ' ',
        .description = "Emergency stop",
        .callback = []()
        {
            std::cout << "Click" << std::endl;
        }
    };

    const std::vector<gui::GUIEntry> entries = {rpm, current};
    const std::vector<gui::KeybindEntry> keybinds = {eStop};
    const auto gui = std::make_shared<gui::SimpleGUI>(entries, keybinds);

    while(!interrupted)
    {
        gui->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
