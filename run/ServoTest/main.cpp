//
// Created by andyh on 1/26/23.
//

#include <servos/src/ClearPathHubManager.h>

// STL
#include <memory>
#include <thread>
#include <atomic>
#include <csignal>
#include <chrono>
#include <iostream>


using namespace evgp_cart::servos;

std::atomic<bool> done = false;
void terminate(int)
{
    done = true;
}

int main()
{
    std::signal(SIGTERM, terminate);

    const auto manager = std::make_shared<ClearPathHubManager>();

    std::atomic<bool> inputProcessed = false;
    std::atomic<char> input = '\0';

    int index = 0;
    int pos0 = 0;
    int pos1 = 0;

    const int increment = 100;

    auto inputThread = std::thread([&](){
        if(done)
            return;

        char newInput = (char)std::getchar();

        input = newInput;
        inputProcessed = false;
    });

    while(!done)
    {
        // Handle input
        if(!inputProcessed)
        {
            if(input == '\t')
                index = (index + 1) % 2;
            else if(input == 'q')
                pos0 += increment;
            else if(input == 'a')
                pos0 -= increment;
            else if(input == 'w')
                pos1 += increment;
            else if(input == 's')
                pos1 -= increment;
        }

        // Update positions
        manager->moveToPositionAbsolute(0, pos0);
        manager->moveToPositionAbsolute(1, pos1);

        // Print
        std::cout << "\x1B[2J\x1B[H";
        std::cout << "Servo 0 position: " << manager->getPositionAbsolute(0) << std::endl;
        std::cout << "Servo 1 position: " << manager->getPositionAbsolute(1) << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if(inputThread.joinable())
        inputThread.join();

    return 0;
}
