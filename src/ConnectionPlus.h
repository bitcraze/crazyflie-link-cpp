#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>
#include <vector>
#include "crazyflieLinkCpp/Connection.h"



class ConnectionPlus : public bitcraze::crazyflieLinkCpp::Connection
{
private:
    /* data */
public:
    ConnectionPlus(/* args */);
    ~ConnectionPlus();
};

// ConnectionPlus::ConnectionPlus(/* args */)
// {
// }

// ConnectionPlus::~ConnectionPlus()
// {
// }
