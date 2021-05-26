#pragma once

#include "ConnectionWrapper.h"

class Crazyflie
{
private:
    ConnectionWrapper _conWrapper;
    bitcraze::crazyflieLinkCpp::Connection _con;  
public:
    Crazyflie(const std::string &uri);
    ~Crazyflie();
};


