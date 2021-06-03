#pragma once

#include <map>
#include <fstream> //for saving .csv file
#include "ConnectionWrapper.h"
#include "Toc.h"

#define PAYLOAD_VALUE_BEGINING_INDEX 3
#define NOT_FOUND 0

#define APP_CHANNEL 2

#define TOC_CHANNEL 0
#define PARAM_READ_CHANNEL 1
#define PARAM_WRITE_CHANNEL 2

#define CMD_TOC_ELEMENT 0 // original version: up to 255 entries
#define CMD_TOC_INFO 1    // original version: up to 255 entries
#define CMD_TOC_ITEM_V2 2 // version 2: up to 16k entries
#define CMD_TOC_INFO_V2 3 // version 2: up to 16k entries

#define PARAM_PORT 2
#define APPCHANNEL_PORT 13

class Crazyflie
{
private:
    bitcraze::crazyflieLinkCpp::Connection _con;
    ConnectionWrapper _conWrapperParamRead;
    ConnectionWrapper _conWrapperParamWrite;
    ConnectionWrapper _conWrapperToc;
    ConnectionWrapper _conWrapperAppchannel;
    bool _isRunning;
    Toc _toc;
    bool setParamInCrazyflie(uint16_t paramId, float newValue);
    bool setParamInCrazyflie(uint16_t paramId, uint32_t newValue, const size_t &valueSize);

    uint32_t getUIntFromCrazyflie(uint16_t paramId) const;
    float getFloatFromCrazyflie(uint16_t paramId) const;
    void initToc();
    TocItem getTocItemFromCrazyflie(uint16_t id) const;

public:
    Crazyflie(const std::string &uri);
    ~Crazyflie();
    bool isRunning() const;
    bool init();

    bool isParamFloat(const std::string &group, const std::string &name) const;

    uint32_t getUIntByName(const std::string &group, const std::string &name) const;
    float getFloatByName(const std::string &group, const std::string &name) const;

    bool setParamByName(const std::string &group, const std::string &name, float newValue);
    bool setParamByName(const std::string &group, const std::string &name, uint32_t newValue, const size_t &valueSize);

    void printToc();
    std::vector<std::pair<TocItem, ParamValue>> getTocAndValues() const;

    void sendAppChannelData(const void *data, const size_t &dataLen);
    std::vector<uint8_t> recvAppChannelData();
    //returns the amount of bytes it wrote
    size_t recvAppChannelData(void* dest, const size_t& dataLen);

    //todo: add callback for param changed
    //todo: console
};
