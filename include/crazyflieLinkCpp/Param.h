#pragma once
#include <iostream>
#include <sstream>
#include <cstring>

#include "ConnectionWrapper.h"

#define PARAM_READ_CHANNEL 1

class Param
{
private:
    ConnectionWrapper _conWrapper;
    bitcraze::crazyflieLinkCpp::Connection& _con;

public:
    Param(bitcraze::crazyflieLinkCpp::Connection &con);
    ~Param();

    float getFloat(uint16_t paramId);
    uint32_t getUInt(uint16_t paramId);
   
};