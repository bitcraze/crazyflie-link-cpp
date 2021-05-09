#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>
#include <vector>
#include "Connection.h"

#define PAYLOAD_MAX_SIZE CRTP_MAXSIZE - 2

#define PARAM_PORT 2
// #pragma push(pack)
// #pragma pack(1)
struct TocItemMessage
{
    uint8_t _cmd;
    uint16_t _id;
};
// #pragma pop(pack)

class ConnectionWrapper
{
private:
    bitcraze::crazyflieLinkCpp::Packet _packet;
    bitcraze::crazyflieLinkCpp::Connection &_con;

public:
    ConnectionWrapper(bitcraze::crazyflieLinkCpp::Connection &con);

    ~ConnectionWrapper();
    bitcraze::crazyflieLinkCpp::Connection &getConnection();
    void setPort(int port);
    void setChannel(int channel);
    void sendData(uint32_t data1, size_t data1_len, uint32_t data2 = 0, size_t data2_len = 0);
};
