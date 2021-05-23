#pragma once
#include <iostream>
#include <sstream>
#include <cstring>

#include "ConnectionWrapper.h"
#include "crazyflieLinkCpp/Toc.h"

#define PAYLOAD_VALUE_BEGINING_INDEX 3
#define NOT_FOUND (-9.99)
#define PARAM_READ_CHANNEL 1

class Param
{
private:
    ConnectionWrapper _conWrapper;
    bitcraze::crazyflieLinkCpp::Connection& _con;
    Toc toc;

public:
    Param(bitcraze::crazyflieLinkCpp::Connection &con);
    ~Param();

    uint32_t getUInt(uint16_t paramId);
    float getFloat(uint16_t paramId);
    float getById(short paramId);
    float getByName(std::string group, std::string name);

    Toc getToc();
};