#pragma once
#include "crazyflieLinkCpp/Connection.h"

class PacketUtils
{
public:
    // Constructs a high level commander takeoff packet
    static bitcraze::crazyflieLinkCpp::Packet takeoffCommand(float height, float yaw, float time) {
        const uint8_t size = 16;
        std::array<uint8_t, size> buffer;
        buffer[0] = 0xFF;
        uint8_t index = 1;
        index += pack(buffer.data(), index, (uint8_t)7);    // Command (Takeoff = 7)
        index += pack(buffer.data(), index, (uint8_t)0);    // Group Mask (0 = all groups)
        index += pack(buffer.data(), index, height);        // Height
        index += pack(buffer.data(), index, yaw);           // Yaw
        index += pack(buffer.data(), index, true);          // Use Current Yaw
        index += pack(buffer.data(), index, time);          // Duration (seconds)
        
        bitcraze::crazyflieLinkCpp::Packet packet(buffer.data(), size);
        packet.setPort((uint8_t)0x08);          // PORT 8 = HIGH_LEVEL_COMMANDER
        return packet;
    }

    // Constructs a high level commander land packet
    static bitcraze::crazyflieLinkCpp::Packet landCommand(float height, float yaw, float time) {
        const uint8_t size = 16;
        std::array<uint8_t, size> buffer;
        buffer[0] = 0xFF;
        uint8_t index = 1;
        index += pack(buffer.data(), index, (uint8_t)8);    // Command (Land = 8)
        index += pack(buffer.data(), index, (uint8_t)0);    // Group Mask (0 = all groups)
        index += pack(buffer.data(), index, height);        // Height
        index += pack(buffer.data(), index, yaw);           // Yaw
        index += pack(buffer.data(), index, true);          // Use Current Yaw
        index += pack(buffer.data(), index, time);          // Duration (seconds)
        
        bitcraze::crazyflieLinkCpp::Packet packet(buffer.data(), size);
        packet.setPort((uint8_t)0x08);          // PORT 8 = HIGH_LEVEL_COMMANDER
        return packet;
    }

    // Constructs a high level commander stop packet
    static bitcraze::crazyflieLinkCpp::Packet stopCommand() {
        const uint8_t size = 3;
        std::array<uint8_t, size> buffer;
        buffer[0] = 0xFF;
        uint8_t index = 1;
        index += pack(buffer.data(), index, (uint8_t)3);    // Command (Stop = 3)
        index += pack(buffer.data(), index, (uint8_t)0);    // Group Mask (0 = all groups)

        bitcraze::crazyflieLinkCpp::Packet packet(buffer.data(), size);
        packet.setPort((uint8_t)0x08);          // PORT 8 = HIGH_LEVEL_COMMANDER
        return packet;
    }

private:
    static size_t pack(uint8_t* buffer, uint8_t index, float value) {
        union {
            float value;
            unsigned char bytes[4];
        } converter;
        converter.value = value;
        buffer[index + 0] = converter.bytes[0];
        buffer[index + 1] = converter.bytes[1];
        buffer[index + 2] = converter.bytes[2];
        buffer[index + 3] = converter.bytes[3];
        return 4;
    }

    static size_t pack(uint8_t* buffer, uint8_t index, uint8_t value) {
        buffer[index] = value;
        return 1;
    }

    static size_t pack(uint8_t* buffer, uint8_t index, bool value) {
        uint8_t byteVal = (value) ? 1 : 0;
        buffer[index] = byteVal;
        return 1;
    }
};