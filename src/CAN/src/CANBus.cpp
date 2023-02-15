//
// Created by andyh on 1/22/23.
//

#include "CANBus.h"

// STL
#include <iostream>

using namespace evgp_project::can;
using namespace std::chrono_literals;

CANBus::CANBus(std::string interface)
        : _interface(std::move(interface))
{
    // Create socketcan socket using RAW protocol
    _fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_fd < 0)
    {
        throw std::runtime_error("[CANBus] Failed to open CAN bus \'" + _interface + "\"");
    }

    ifreq ifr;
    sockaddr_can addr;

    std::strcpy(ifr.ifr_name, _interface.c_str());
    ioctl(_fd, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(_fd, (sockaddr *) &addr, sizeof(addr)) < 0)
    {
        throw std::runtime_error("[CANBus] Failed to configure CAN bus \'" + _interface + "\"");
    }

    _incomingFramesThread = std::thread([this]() { getIncomingFrames(); });
}

CANBus::~CANBus()
{
    close(_fd);

    if (_incomingFramesThread.joinable())
        _incomingFramesThread.join();
}

bool CANBus::subscribeFrameID(const uint32_t frameID, std::function<void(can_frame)> callbackFunction)
{
    bool success;

    // Mutex scope
    {
        std::scoped_lock<std::mutex> lock(_subscribedFrameCallbackMutex);

        // Insert new frame into callback map
        const auto iterator = _subscribedFrameCallbacks.insert({frameID, callbackFunction});
        success = iterator.second;
    }

    if (!success)
    {
        std::cout << "Unable to subscribe frame ID " << frameID << " to CAN bus {}" << _interface << std::endl;
    }

    return success;
}

bool CANBus::sendFrame(const uint32_t canID, const std::vector<std::uint8_t>& payload)
{
    can_frame frame;

    frame.can_id = canID;
    frame.can_dlc = payload.size();

    // Fill frame payload
    for (std::size_t i = 0; i < payload.size(); i++)
    {
        frame.data[i] = payload[i];
    }

    return writeFrame(frame);
}

std::chrono::system_clock::time_point CANBus::timevalToTimePoint(const timeval& timeval)
{
    using namespace std::chrono;

    time_point<system_clock, milliseconds> converted{milliseconds{timeval.tv_sec * 1000 + timeval.tv_usec / 1000}};
    return time_point_cast<system_clock::duration>(converted);
}

std::string CANBus::getInterface() const
{
    return _interface;
}

void CANBus::getIncomingFrames()
{
    while (true)
    {
        // Read frames from bus
        BasicRead reading = readFrame();

        if (reading.isValid)
        {
            // Check if payload is correct size
//            if (reading.frame.can_dlc != 8)
//            {
//                std::cout << "Received CAN message (canID: " << reading.frame.can_id << ") with payload size " << reading.frame.can_dlc << ". (should be 8)" << std::endl;
//                continue;
//            }

            // Packet age check
            auto difference = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now() - reading.timestamp);
            if (difference > 100ms)
            {
                std::cout << "Processing CAN Frame older than 100ms. (" << difference.count() << " ms old)" << std::endl;
            }

            // Call callback from map
            uint32_t receivedFrameID = reading.frame.can_id;

            // Mutex scope
            {
                std::unique_lock<std::mutex> lock(_subscribedFrameCallbackMutex);

                const auto& it = _subscribedFrameCallbacks.find(receivedFrameID);
                if (it != _subscribedFrameCallbacks.end())
                {
                    it->second(reading.frame);  // Run callback with frame
                }
            }

        }

        std::this_thread::sleep_for(100us);
    }
}

BasicRead CANBus::readFrame() const
{
    can_frame frame;
    std::chrono::system_clock::time_point timestamp;

    // Calculate seconds and ms from timeout_ms
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    // Watch _fd to see when it has input. This must be done on EVERY read
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(_fd, &readfds);

    // Check to see if there is data to be read on the canbus
    int retval = select(_fd + 1, &readfds, nullptr, nullptr, &timeout);

    // If there is data to be read, read it
    if (retval > 0)
    {
        std::size_t nbytes = read(_fd, &frame, sizeof(can_frame));

        // If no data loss, get timestamp and return
        if (nbytes == sizeof(can_frame))
        {
            // Get time that frame was received
            timeval timestampRaw;

#ifdef SIOCGSTAMP_OLD
            ioctl(_fd, SIOCGSTAMP_OLD, &timestampRaw);
#else
            ioctl(_fd, SIOCGSTAMP, &timestampRaw);
#endif

            timestamp = timevalToTimePoint(timestampRaw);

            return {true, timestamp, frame};
        }
    }

    return {false, {}, {}};
}

bool CANBus::writeFrame(const can_frame& frame) const
{
    // Attempt to write frame to bus _maxRetries times
    int tries = _kMaxRetries;
    do
    {
        {
            std::lock_guard<std::mutex> lock(_busMutex);

            // Check to make sure number of bytes written matches size of frame
            int num_bytes = write(_fd, &frame, sizeof(can_frame));
            if(num_bytes == sizeof(frame))
            {
                return true;
            }
            else
            {
                std::cout << "Wrote " << num_bytes << " bytes, when we were supposed to write " << sizeof(frame) << "!" << std::endl;
            }
        }

        tries--;

        std::cout << "Individual write failed on can bus " << _interface << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } while (tries >= 0);

    std::cout << "Failed to write CAN frame!\n\tInterface: " << _interface << "\n\tID: " << frame.can_id << std::endl;

    return false;
}
