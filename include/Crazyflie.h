#pragma once

#include <map>
#include <fstream> //for saving .csv file
#include "ConnectionWrapper.h"

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

#include "Toc.h"



class Crazyflie
{
private:

    bitcraze::crazyflieLinkCpp::Connection _con;
    ConnectionWrapper _conWrapperParamRead;
    ConnectionWrapper _conWrapperParamWrite;
    ConnectionWrapper _conWrapperToc;
    ConnectionWrapper _conWrapperAppchannel;


    void saveToc(const std::string& filename) const;  //save the TOC to a .csv file
    void loadToc(const std::string& filename);  //save the TOC to a .csv file

    // app channel read file via param change
    //////////////////////////////////////////////////





    //private ?
    uint32_t getUInt(uint16_t paramId) const;
    float getFloat(uint16_t paramId) const;
    float getFloatById(uint16_t paramId) const;
    uint32_t getUIntById(uint16_t paramId) const;

public:
    Toc _toc;

    Crazyflie(const std::string& uri);
    ~Crazyflie();

    // GOOD API
    uint32_t getUIntByName(const std::string& group, const std::string& name) const;
    float getFloatByName(const std::string& group, const std::string& name) const;

    // QUESTIONABLE API - use BY NAME
    bool setParam(uint16_t paramId, float newValue);
    bool setParam(uint16_t paramId, uint32_t newValue, const size_t& valueSize);

    bool init();
    // bool init(bool paramChangedCB);
    //todo: add init ? bool init(paramChangedCB)
    //todo: add callback for param changed
    
    //where is Console? 



    // does not help me
    void initToc();
    TocItem getItemFromToc(uint16_t id) const;
    static std::pair<int, std::string> getAccessAndStrType(uint8_t type) ;

    std::vector<uint8_t> recvAppChannelData();

    void printToc(); //print the TOC to the console


    static std::string accessTypeToStr(int accessType) ;


    void sendAppChannelData(const void* data, const size_t& dataLen);
};

