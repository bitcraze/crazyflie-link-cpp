#pragma once

#include <map>
#include <fstream> //for saving .csv file
#include "ConnectionWrapper.h"

#define PAYLOAD_VALUE_BEGINING_INDEX 3
#define NOT_FOUND 0

#define APP_CHANNEL 

#define TOC_CHANNEL 0
#define PARAM_READ_CHANNEL 1
#define PARAM_WRITE_CHANNEL 2

#define CMD_TOC_ELEMENT 0 // original version: up to 255 entries
#define CMD_TOC_INFO 1    // original version: up to 255 entries
#define CMD_TOC_ITEM_V2 2 // version 2: up to 16k entries
#define CMD_TOC_INFO_V2 3 // version 2: up to 16k entries

#define RW_ACCESS 0
#define RO_ACCESS 1
#define ACCESS_TYPE_BYTE 64

#define PARAM_PORT 2
#define APPCHANNEL_PORT 13

    typedef std::pair<std::string, std::string> StrPair;
    typedef std::pair<uint8_t, StrPair> TypeMapPair;

const std::map<uint8_t, StrPair>
        PARAM_TYPES = {//          TypeId     TypeName
                 TypeMapPair(0x08, StrPair({"uint8_t", "<B"})),
                 TypeMapPair(0x09, StrPair({"uint16_t", "<H"})),
                 TypeMapPair(0x0A, StrPair({"uint32_t", "<L"})),
                 TypeMapPair(0x0B, StrPair({"uint64_t", "<Q"})),
                 TypeMapPair(0x00, StrPair({"int8_t", "<b"})),
                 TypeMapPair(0x01, StrPair({"int16_t", "<h"})),
                 TypeMapPair(0x02, StrPair({"int32_t", "<i"})),
                 TypeMapPair(0x03, StrPair({"int64_t", "<q"})),
                 TypeMapPair(0x05, StrPair({"FP16", ""})),
                 TypeMapPair(0x06, StrPair({"float", "<f"})),
                 TypeMapPair(0x07, StrPair({"double", "<d"}))};

struct TocItem
{
    std::string _groupName;
    std::string _paramName;
    uint8_t _cmdNum;
    uint8_t _paramType;
    uint16_t _paramId;

    TocItem(const bitcraze::crazyflieLinkCpp::Packet& p_recv);
    friend std::ostream &operator<<(std::ostream& out, const TocItem& tocItem);
};

struct TocInfo
{
    uint16_t _numberOfElements;
    uint32_t _crc;

    TocInfo(const bitcraze::crazyflieLinkCpp::Packet& p_recv);

    friend std::ostream &operator<<(std::ostream& out, const TocInfo& tocInfo);
};

class Crazyflie
{
private:

    bitcraze::crazyflieLinkCpp::Connection _con;
    ConnectionWrapper _conWrapperParamRead;
    ConnectionWrapper _conWrapperParamWrite;
    ConnectionWrapper _conWrapperToc;
    ConnectionWrapper _conWrapperAppchannel;

public:
    Crazyflie(const std::string& uri);
    ~Crazyflie();

    // GOOD API
    uint32_t getUIntByName(const std::string& group, const std::string& name) const;
    float getFloatByName(const std::string& group, const std::string& name) const;

    // QUESTIONABLE API - use BY NAME
    bool setParam(uint16_t paramId, float newValue);
    bool setParam(uint16_t paramId, uint32_t newValue, const size_t& valueSize);

    bool init(bool paramChangedCB);
    //todo: add init ? bool init(paramChangedCB)
    //todo: add callback for param changed
    
    //where is Console? 


    void saveToc(std::string filename);  //save the TOC to a .csv file

    // app channel read file via param change
    //////////////////////////////////////////////////





    //private ?
    uint32_t getUInt(uint16_t paramId) const;
    float getFloat(uint16_t paramId) const;
    float getFloatById(uint16_t paramId) const;
    uint32_t getUIntById(uint16_t paramId) const;


    // does not help me
    TocInfo getTocInfo() const;
    TocItem getItemFromToc(uint16_t id) const;
    static std::pair<int, std::string> getAccessAndStrType(uint8_t type) ;

    std::vector<TocItem> getToc();

    void printToc(); //print the TOC to the console


    static std::string accessTypeToStr(int accessType) ;


    void sendAppChannelData(const void* data, const size_t& dataLen);
};


