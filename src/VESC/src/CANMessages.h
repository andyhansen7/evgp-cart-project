//
// Created by andyh on 2/14/23.
//

#ifndef EVGP_CART_CANMESSAGES_H
#define EVGP_CART_CANMESSAGES_H

namespace evgp_project::vesc
{
    typedef enum can_messages
    {
        // Status frames
        Status1 = 9,
        Status2 = 14,
        Status3 = 15,
        Status4 = 16,
        Status5 = 27,
        Status6 = 58,   // Only used for ADC / servo values, not implemented

        // Ping-pong
        Ping = 17,
        Pong = 18,

        // Speed control
        SetDutyPercent = 0,
        SetRPM = 3,

        // Current control
        SetCurrentA = 1,
        SetRelativeCurrentA = 10,
        SetCurrentHandbrakeA = 12,
        SetRelativeCurrentHandbrakeA = 13,

        // Brake control
        SetCurrentBrakeA = 2,
        SetRelativeCurrentBrakeA = 11,

        // Other control
        Shutdown = 31,
    } CANMessages;
}
#endif //EVGP_CART_CANMESSAGES_H
