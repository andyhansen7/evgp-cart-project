//
// Created by andyh on 2/14/23.
//

#ifndef EVGP_CART_CANUTILS_H
#define EVGP_CART_CANUTILS_H

// STL
#include <cstdint>
#include <vector>
#include <linux/can.h>

namespace evgp_project::vesc
{
    /** Helper function to build an EFF CAN ID header using the frame id and device id
     *
     * @param frameID the ID of the frame, enumerated in CANMessages.h
     * @param deviceID the ID of the device to target with the messages
     * @return the created header
     */
    static inline uint32_t buildCANExtendedID(const uint32_t frameID, const uint32_t deviceID)
    {
        return ((frameID << 8) | deviceID | CAN_EFF_FLAG);
    }

    /** Helper function to create a vector 8 bytes in length with the provided payload.
     *  This notation is a little messy, but prevents having to re-declare vectors in the
     *  driver implementation
     *
     * @param byte0-7 data as a set of bytes
     * @return a vector of length 8 containing the provided data
     */
    static std::vector<uint8_t> buildCANFramePayload(const uint8_t byte7,
                                                     const uint8_t byte6,
                                                     const uint8_t byte5,
                                                     const uint8_t byte4,
                                                     const uint8_t byte3,
                                                     const uint8_t byte2,
                                                     const uint8_t byte1,
                                                     const uint8_t byte0)
    {
        std::vector<uint8_t> vec;
        vec.resize(8);

        vec[0] = byte0;
        vec[1] = byte1;
        vec[2] = byte2;
        vec[3] = byte3;
        vec[4] = byte4;
        vec[5] = byte5;
        vec[6] = byte6;
        vec[7] = byte7;

        return vec;
    }

    /** Helper methods to apply bit masks so that they do not have to be done manually.
     */
    static inline uint8_t byte0(const uint32_t data) { return data & 0xF; }
    static inline uint8_t byte1(const uint32_t data) { return (data >> 8) & 0xF; }
    static inline uint8_t byte2(const uint32_t data) { return (data >> 16) & 0xF; }
    static inline uint8_t byte3(const uint32_t data) { return (data >> 24) & 0xF; }

    /** Helper function to apply a fixed multiplier to a value, used by the VESC to
     *  emulate the functionality of floating-point values without actually serializing
     *  floats or doubles.
     *
     *  @param data the desired value
     *  @param factor the factor to multiply by
     *
     *  @return the data as a 32-bit unsigned number
     */
     static inline uint32_t applyFactor(const float data, const uint32_t factor)
    {
         return static_cast<uint32_t>(data * factor);
    }
}

#endif //EVGP_CART_CANUTILS_H
