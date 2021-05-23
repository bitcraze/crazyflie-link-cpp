#pragma once

#include <iostream>
#include "Connection.h"

#define PAYLOAD_MAX_SIZE (CRTP_MAXSIZE - 2)
#define PARAM_PORT 2

struct TocItemMessage
{
    uint8_t _cmd;
    uint16_t _id;
};

bool isBigEndian();

const bool IS_BIG_ENDIAN = isBigEndian();
class ConnectionWrapper
{
private:
    bitcraze::crazyflieLinkCpp::Packet _packet;
    bitcraze::crazyflieLinkCpp::Connection &_con;

public:
    ConnectionWrapper(bitcraze::crazyflieLinkCpp::Connection &con);

    bitcraze::crazyflieLinkCpp::Packet recvFilteredData(int timeout, int port, int channel);

    // returns the data only from the same port and channel as the current _packet
    bitcraze::crazyflieLinkCpp::Packet recvFilteredData(int timeout);

    bitcraze::crazyflieLinkCpp::Connection &getConnection();
    void setPort(int port);
    void setChannel(int channel);
    void sendData(uint32_t data1, size_t data1_len, uint32_t data2 = 0, size_t data2_len = 0);
    ~ConnectionWrapper();
};
