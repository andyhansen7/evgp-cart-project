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
        ~ClearPathHubManager();

        ClearPathHubManager& operator=(const ClearPathHubManager& other) = delete;
        ClearPathHubManager(const ClearPathHubManager& other) = delete;

        /** Move the servo to an absolute position, in encoder ticks
         *
         * @param node the index of the node
         * @param encoderTicks the tick count of the target
         * @return false on error
         */
        bool moveToPositionAbsolute(unsigned node, int encoderTicks);

        /** Move the servo to a position relative to the current position, in encoder ticks
         *
         * @param node the index of the node
         * @param encoderTicks the tick count of the target
         * @return false on error
         */
        bool moveToPositionRelative(unsigned node, int encoderTicks);

        int getPositionAbsolute(unsigned node);

    private:
        // Manager instance
        sFnd::SysManager* _manager;

        // Port names found by the hub
        std::vector<std::string> _portNames;

        // Constants
        static constexpr unsigned _enableTimeout_ms = 10000;    // 10 seconds
        static constexpr unsigned _homingTimeout_ms = 10000;    // 10 seconds
        static constexpr double _accelerationLimit_rpm_sec = 100000;
        static constexpr double _velocityLimit_rpm = 700;
    };
}


#endif //EVGP_CART_CLEARPATHHUBMANAGER_H
