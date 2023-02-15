//
// Created by andyh on 2/14/23.
//

#include "SimpleGUI.h"

// STL
#include <iostream>

using namespace evgp_project::gui;

SimpleGUI::SimpleGUI(std::vector<GUIEntry> entries,
                     std::vector<KeybindEntry> keybinds)
                   : _entries(std::move(entries)),
                     _keybinds(std::move(keybinds))
{
    // Create input thread
    _inputThreadRunning = true;
    _inputThread = std::thread([this](){ getInput(); });

    // Screen initialization
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    refresh();

    // Run initial display
    update();
}

SimpleGUI::~SimpleGUI()
{
    echo();
    endwin();
    std::cin.putback('\0');

    _inputThreadRunning = false;

    if(_inputThread.joinable())
    {
        _inputThread.join();
    }

}

void SimpleGUI::update()
{
    // Clear terminal
    clear();

    // Keep track of current row that we are writing to
    int row = 0;

    // Print header
    mvprintw(row, _colOffsets[0], "Data Fields");
    mvprintw(row, _colOffsets[1], "Key Bindings");
    row++;

    for(unsigned i = 0; i < _entries.size(); i++)
    {
        const auto entry = _entries[i];
        const std::string line = entry.name + ": " + toStringWithPrecision(entry.value(), entry.precision);

        mvprintw(i + row, _colOffsets[0], line.c_str());
    }

    for(unsigned i = 0; i < _keybinds.size(); i++)
    {
        const auto binding = _keybinds[i];

        std::stringstream line;
        line << "\'" << binding.key << "\'" <<  " - " << binding.description;

        mvprintw(i + row, _colOffsets[1], line.str().c_str());
    }
}

void SimpleGUI::getInput()
{
    while(_inputThreadRunning)
    {
        // Delay a little
        std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(_inputDelay_ms));

        // Get input from command line
        char input = getch();
        fflush(stdin);

        // Interpret
        for(auto& binding : _keybinds)
        {
            if(input == binding.key)
            {
                binding.callback();
            }
        }
    }
}

