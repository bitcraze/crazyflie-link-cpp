#pragma once
#include <iostream>
#include <sstream>
#include <cstring>

#include "ConnectionWrapper.h"
#include "Toc.h"

#define PAYLOAD_VALUE_BEGINING_INDEX 3
#define NOT_FOUND 0
#define PARAM_READ_CHANNEL 1
#define PARAM_WRITE_CHANNEL 2

class Param
{
private:
    ConnectionWrapper _conWrapper;
    bitcraze::crazyflieLinkCpp::Connection& _con;
    Toc _toc;

public:

    Param(bitcraze::crazyflieLinkCpp::Connection &con);
    ~Param();

    uint32_t getUInt(uint16_t paramId);
    float getFloat(uint16_t paramId);
    float getFloatById(uint16_t paramId);
    uint32_t getUIntById(uint16_t paramId);
    uint32_t getUIntByName(std::string group, std::string name);
    float getFloatByName(std::string group, std::string name);

    bool setParam(uint16_t paramId, float newValue);
    bool setParam(uint16_t paramId, uint32_t newValue, size_t valueSize);
    
    Toc getToc();
};