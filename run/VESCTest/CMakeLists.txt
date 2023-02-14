add_executable(
    VESCTest

    main.cpp
)

target_link_libraries(
    VESCTest

    CAN
    VESC
)