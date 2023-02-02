//
// Created by andyh on 1/27/23.
//

#include "ClearPathHubManager.h"

// STL
#include <iostream>

using namespace evgp_cart::servos;

ClearPathHubManager::ClearPathHubManager()
    : _manager(sFnd::SysManager::Instance())
{
    // Locate hubs connected over USB
    try {
        sFnd::SysManager::FindComHubPorts(_portNames);
    } catch(std::exception&) {
        std::cerr << "ERROR: Failed to find COM hub ports for manager" << std::endl;
        return;
    }

    if(_portNames.size() != 1)
    {
        std::cerr << "ERROR: Located " << _portNames.size() << " SC hubs (should be 1)" << std::endl;
        return;
    }

    // Set up the port to the hub
    _manager->ComHubPort(1, _portNames[0].c_str());
    auto& port = _manager->Ports(0);

    // Print debug info for the port
    std::cout << "Port state: " << port.OpenState() << "\nNode count: " << port.NodeCount() << std::endl;

    // Create node references
    for(unsigned i = 0; i < port.NodeCount(); i++)
    {
        auto& node = port.Nodes(i);
        node.EnableReq(false);
        _manager->Delay(200);

        std::cout << "Node #" << i << "\n\t"
                  << "Type: " << node.Info.NodeType() << "\n\t"
                  << "userID: " << node.Info.UserID.Value() << "\n\t"
                  << "FW version: " << node.Info.FirmwareVersion.Value() << "\n\t"
                  << "Serial: " << node.Info.SerialNumber.Value() << "\n\t"
                  << "Model: " << node.Info.Model.Value() << std::endl;

        // Clear alerts and stops
        node.Status.AlertsClear();
        node.Motion.NodeStopClear();

        // Set units
        node.AccUnit(sFnd::INode::RPM_PER_SEC);
        node.VelUnit(sFnd::INode::RPM);

        // Set limits
        node.Motion.AccLimit = _accelerationLimit_rpm_sec;
        node.Motion.VelLimit = _velocityLimit_rpm;

        node.EnableReq(true);
        std::cout << "\n\tNode enabled." << std::endl;

        double timeout = _manager->TimeStampMsec() + _enableTimeout_ms;
        while(!node.Motion.IsReady())
        {
            if(_manager->TimeStampMsec() > timeout)
            {
                std::cerr << "ERROR: Failed to enable node #" << i << std::endl;
                return;
            }
        }

        if(node.Motion.Homing.HomingValid())
        {
            if(node.Motion.Homing.WasHomed())
            {
                std::cout << "Node already homed, re-homing" << std::endl;
            }
            else
            {
                std::cout << "Homing node" << std::endl;
            }

            node.Motion.Homing.Initiate();

            timeout = _manager->TimeStampMsec() + _homingTimeout_ms;
            while(!node.Motion.Homing.WasHomed())
            {
                if(_manager->TimeStampMsec() > timeout)
                {
                    std::cerr << "ERROR: Failed to home node #" << i << std::endl;
                    return;
                }
            }
            std::cout << "\tHoming complete!" << std::endl;
        }
        else
        {
           std::cout << "Homing is not enabled for this servo. Moving on" << std::endl;
        }
    }
}

ClearPathHubManager::~ClearPathHubManager()
{
    auto& port = _manager->Ports(0);
    for(unsigned i = 0; i < port.NodeCount(); i++) {
        port.Nodes(i).EnableReq(false);
    }
    std::cout << "Servos disabled." << std::endl;

    _manager->PortsClose();
    std::cout << "Manager shut down." << std::endl;
}

bool ClearPathHubManager::moveToPositionAbsolute(const unsigned node, const int encoderTicks)
{
    auto& port = _manager->Ports(0);
    try {
        auto& nodeInstance = port.Nodes(node);
        nodeInstance.Motion.MovePosnStart(encoderTicks, true);
        // TODO: start a watchdog timer here
        return true;
    } catch(std::exception&) {
        return false;
    }
}

bool ClearPathHubManager::moveToPositionRelative(unsigned int node, int encoderTicks)
{
    auto& port = _manager->Ports(0);
    try {
        auto& nodeInstance = port.Nodes(node);
        nodeInstance.Motion.MovePosnStart(encoderTicks);
        // TODO: start a watchdog timer here
        return true;
    } catch(std::exception&) {
        return false;
    }
}

int ClearPathHubManager::getPositionAbsolute(unsigned node)
{
    auto& port = _manager->Ports(0);
    try {
        auto& nodeInstance = port.Nodes(node);
        return static_cast<int>(nodeInstance.Motion.PosnMeasured.Value());
    } catch(std::exception&) {
        return 0;
    }
}