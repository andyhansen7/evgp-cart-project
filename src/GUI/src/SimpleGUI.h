//
// Created by andyh on 2/14/23.
//

#ifndef EVGP_CART_SIMPLEGUI_H
#define EVGP_CART_SIMPLEGUI_H

// Curses
#include <ncurses.h>

// STL
#include <string>
#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <sstream>

namespace evgp_project::gui
{
    typedef struct gui_entry
    {
        std::string name;
        unsigned precision;
        std::function<double(void)> value;
    } GUIEntry;

    typedef struct keybind_entry
    {
        char key;
        std::string description;
        std::function<void(void)> callback;
    } KeybindEntry;

    class SimpleGUI
    {
    public:
        SimpleGUI(std::vector<GUIEntry> entries, std::vector<KeybindEntry> keybinds);

        ~SimpleGUI();

        // Needs to be called to update the display state
        void update();

    private:
        std::vector<GUIEntry> _entries;
        std::vector<KeybindEntry> _keybinds;

        // Constants for column positions
        const std::vector<int> _colOffsets = {0, 50};

        // Input thread
        std::atomic<bool> _inputThreadRunning;
        std::thread _inputThread;
        const float _inputDelay_ms = 100;

        // Input thread method
        void getInput();

        // Helper to update the precision of a number and return the string
        static inline std::string toStringWithPrecision(const float value, const unsigned precision = 6)
        {
            std::ostringstream out;
            out.precision(precision);
            out << std::fixed << value;
            return out.str();
        }
    };
}

#endif //EVGP_CART_SIMPLEGUI_H
