//
// Created by andyh on 1/22/23.
//

// linux
#include <linux/can.h>

// sys
#include <sys/socket.h>
#include <sys/ioctl.h>

// net
#include <net/if.h>

// STL
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <cstring>
#include <map>
#include <thread>
#include <functional>
#include <queue>
#include <sstream>
#include <unistd.h>
#include <cstdint>
#include <memory>

#ifndef VESC_CAN_DRIVER_CANBUS_H
#define VESC_CAN_DRIVER_CANBUS_H

struct BasicRead
{
    const bool isValid;
    const std::chrono::system_clock::time_point timestamp;
    const can_frame frame;
};

class CANBus
{
public:
    explicit CANBus(std::string interface);
    ~CANBus();

    CANBus(const CANBus& other) = delete;
    CANBus& operator=(CANBus other) = delete;

    // iCANBus overrides
    bool subscribeFrameID(const uint32_t frameID, std::function<void(can_frame frame)> callbackFunction);
    bool sendFrame(const uint32_t canID, const std::vector<std::uint8_t>& payload);
    std::string getInterface() const;

private:
    // CAN bus objects
    const std::string _interface;

    mutable std::mutex _busMutex;
    int _fd;

    // Threads
    std::thread _incomingFramesThread;

    // Subscribed CAN ID objects
    mutable std::mutex _subscribedFrameCallbackMutex;
    std::map<uint32_t, std::function<void(can_frame frame)>> _subscribedFrameCallbacks;

    // Max retries for sending a frame
    static constexpr int _kMaxRetries = 5;

    // Function called by receiving thread to call subscribed callbacks when a frame with the correct ID is received
    void getIncomingFrames();

    // Helper function to convert timeval structure to chrono timepoint
    static std::chrono::system_clock::time_point timevalToTimePoint(const timeval& timeval);

    /// Read frame from canbus
    /// \return BasicRead struct of frame read
    BasicRead readFrame() const;

    /// Write frame to canbus
    /// \param frame - prebuilt frame to be sent
    /// \return true on success
    bool writeFrame(const can_frame& frame) const;

    // Helper function to convert literal type to hex string (useful for debugging and log messages)
    template<typename T>
    static std::string toHexString(T value)
    {
        std::stringstream stream;
        stream << "0x" << std::hex << value;
        return stream.str();
    }
};

#endif //VESC_CAN_DRIVER_CANBUS_H
