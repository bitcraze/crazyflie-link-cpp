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



class ConnectionPlus : public bitcraze::crazyflieLinkCpp::Connection
{
private:
    bitcraze::crazyflieLinkCpp::Packet _packet;
public:
    ConnectionPlus(std::string uri);
    ~ConnectionPlus();

    void setPort(int port);
    void setChannel(int channel);
    void sendInt(uint8_t intigerToSend);
    void sendInt(uint8_t intigerToSend, uint16_t extraData);
    
    void sendInt(uint8_t intigerToSend, int port, int channel);
    void sendInt(uint8_t intigerToSend, uint16_t extraData, int port, int channel);
};
