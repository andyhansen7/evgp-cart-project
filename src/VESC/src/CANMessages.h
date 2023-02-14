//
// Created by andyh on 2/14/23.
//

#ifndef EVGP_CART_CANMESSAGES_H
#define EVGP_CART_CANMESSAGES_H

namespace evgp_cart::vesc
{
    typedef enum can_messages
    {
        SetDutyPercent = 0,
        SetCurrentA = 1,
        Ping = 17,
        Pong = 18
    } CANMessages;
}
#endif //EVGP_CART_CANMESSAGES_H
