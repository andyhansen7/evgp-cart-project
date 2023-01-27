//
// Created by andyh on 1/27/23.
//

#include "ClearPathHubManager.h"

// STL
#include <iostream>

using namespace evgp_car::servos;

ClearPathHubManager::ClearPathHubManager()
    : _manager(sFnd::SysManager::Instance())
{
    // Locate hubs connected over USB
    sFnd::SysManager::FindComHubPorts(_portNames);

    if(_portNames.size() != 1)
    {
        std::cerr << "ERROR: Located " << _portNames.size() << " SC hubs (should be 1)" << std::endl;
        return;
    }

    // Set up the port to the hub
    _manager->ComHubPort(1, _portNames[0].c_str());
    _port = myMgr->Ports(0);

    // Print debug info for the port
    std::cout << "Port state: " << _port.OpenState() << "\nNode count: " << _port.NodeCount() << std::endl;
}