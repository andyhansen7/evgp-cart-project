//
// Created by andyh on 1/27/23.
//

#ifndef EVGP_CART_CLEARPATHHUBMANAGER_H
#define EVGP_CART_CLEARPATHHUBMANAGER_H

// Clearpath SDK
#include "pubSysCls.h"

// STL
#include <vector>
#include <string>

/**
 * #define ACC_LIM_RPM_PER_SEC	100000
#define VEL_LIM_RPM			700
#define MOVE_DISTANCE_CNTS	10000
#define NUM_MOVES			5
#define TIME_TILL_TIMEOUT	10000	//The timeout used for homing(ms)
 */

namespace evgp_cart::servos
{
    class ClearPathHubManager
    {
    public:
        ClearPathHubManager();
        ~ClearPathHubManager() = default;

        ClearPathHubManager& operator=(const ClearPathHubManager& other) = delete;
        ClearPathHubManager(const ClearPathHubManager& other) = delete;

    private:
        // Manager instance
        sFnd::SysManager* _manager;

        // Port names found by the hub
        std::vector<std::string> _portNames;

    };
}


#endif //EVGP_CART_CLEARPATHHUBMANAGER_H
