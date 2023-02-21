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
std::shared_ptr<vesc::VESC> controller;

void handler(int)
{
    interrupted = true;
    throw std::runtime_error("Killed.");
}

int main()
{
    signal(SIGINT, handler);

    static constexpr unsigned vescID = 48;

    const auto bus = std::make_shared<can::CANBus>("can0");
    controller = std::make_shared<vesc::VESC>(bus, vescID);

    float pwmTarget = 0.0f;
    const float pwmIncrement = 0.05f;
    const float pwmMax = 1.0f;
    const float pwmMin = -1.0f;

    float rpmTarget = 0.0f;
    float rpmIncrement = 100.0f;
    float rpmMax = 5000.0f;
    float rpmMin = -5000.0f;

    gui::GUIEntry setRPM = {
        .name = "RPM Target",
        .precision = 6,
        .value = [&]() {
            return rpmTarget;
        }
    };

    gui::GUIEntry responseRate = {
        .name = "Response (%)",
        .precision = 6,
        .value = [&]() {
            return static_cast<float>(controller->getPongs()) / static_cast<float>(controller->getPings());
        }
    };

    gui::GUIEntry rpm = {
        .name = "RPM",
        .precision = 6,
        .value = [&]() {
            return controller->getRPM();
        }
    };

    gui::GUIEntry dutyPercent = {
        .name = "Duty (%)",
        .precision = 6,
        .value = [&]() {
            return controller->getDutyCycle();
        }
    };

    gui::GUIEntry current = {
        .name = "Current (A)",
        .precision = 6,
        .value = [&]() {
            return controller->getOutputCurrent();
        }
    };

    gui::GUIEntry totalAH = {
        .name = "Total Power (AH)",
        .precision = 6,
        .value = [&]() {
            return controller->getTotalConsumedAmpHours();
        }
    };

    gui::GUIEntry totalWH = {
        .name = "Total Power (WH)",
        .precision = 6,
        .value = [&]() {
            return controller->getTotalConsumedWattHours();
        }
    };

    gui::GUIEntry chargedAH = {
        .name = "Total Charged (AH)",
        .precision = 6,
        .value = [&]() {
            return controller->getTotalAmpHoursRecharged();
        }
    };

    gui::GUIEntry chargedWH = {
        .name = "Total Charged (WH)",
        .precision = 6,
        .value = [&]() {
            return controller->getTotalWattHoursRecharged();
        }
    };

    gui::GUIEntry mosfetTemp = {
        .name = "MOSFET Temp",
        .precision = 6,
        .value = [&]() {
            return controller->getMosfetTemperature();
        }
    };

    gui::GUIEntry motorTemp = {
        .name = "Motor Temp",
        .precision = 6,
        .value = [&]() {
            return controller->getMotorTemperature();
        }
    };

    gui::GUIEntry voltage = {
        .name = "Input Voltage",
        .precision = 6,
        .value = [&]() {
            return controller->getInputVoltage();
        }
    };

    gui::KeybindEntry eStop = {
        .key = ' ',
        .description = "Emergency stop",
        .callback = [&]()
        {
            controller->setRPM(0.0f);
        }
    };

    gui::KeybindEntry increaseRPM = {
        .key = 'w',
        .description = "Increase RPM",
        .callback = [&]()
        {
            rpmTarget += rpmIncrement;

            if(rpmTarget > rpmMax)
                rpmTarget = rpmMax;

            controller->setRPM(rpmTarget);
        }
    };

    gui::KeybindEntry decreaseRPM = {
        .key = 's',
        .description = "Decrease RPM",
        .callback = [&]()
        {
            rpmTarget -= rpmIncrement;

            if(rpmTarget < rpmMin)
                rpmTarget = rpmMin;

            controller->setRPM(rpmTarget);
        }
    };

    const std::vector<gui::GUIEntry> entries = {setRPM,
                                                responseRate,
                                                rpm,
                                                dutyPercent,
                                                current,
                                                totalAH,
                                                totalWH,
                                                chargedAH,
                                                chargedWH,
                                                mosfetTemp,
                                                motorTemp,
                                                voltage};

    const std::vector<gui::KeybindEntry> keybinds = {eStop,
                                                     increaseRPM,
                                                     decreaseRPM};

    const auto gui = std::make_shared<gui::SimpleGUI>(entries, keybinds);

    while(!interrupted)
    {
        gui->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
