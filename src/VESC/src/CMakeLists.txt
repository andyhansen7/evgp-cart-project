add_library(
    VESC

    VESC.cpp
)

target_link_libraries(
    VESC

    CAN
)