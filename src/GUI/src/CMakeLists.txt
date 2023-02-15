add_library(
    GUI

    SimpleGUI.cpp
)

target_link_libraries(
    GUI

    ${CURSES_LIBRARIES}
)