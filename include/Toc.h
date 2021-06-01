#pragma once

#include <string>
#include <iostream>
#include <map>
#include "Packet.hpp"


#define ACCESS_TYPE_BYTE 64

const std::map<uint8_t, std::string>
        PARAM_TYPES = {
            //   TypeId:       TypeName:
                 {0x08, "uint8_t"},
                 {0x09, "uint16_t"},
                 {0x0A, "uint32_t"},
                 {0x0B, "uint64_t"},
                 {0x00, "int8_t"},
                 {0x01, "int16_t"},
                 {0x02, "int32_t"},
                 {0x03, "int64_t"},
                 {0x05, "FP16"},
                 {0x06, "float"},
                 {0x07, "double"}};

struct AccessType
{
    uint8_t _accessType;
    static const uint8_t RW = 0;
    static const uint8_t RO = 1;
    friend std::string(const AccessType& accessType);
    AccessType& operator=(const std::string& strAccessType);
    AccessType& operator=(const uint8_t& accessType);
};
    static const uint8_t RO = 1;



struct ParamType
{
    uint8_t _paramtype;
    std::string getStrParamType() const;
    AccessType& operator=(const std::string& strAccessType);
    AccessType& operator=(const uint8_t& accessType);
};



struct TocItem
{
    std::string _groupName;
    std::string _paramName;
    uint8_t _paramType;
    AccessType _paramAccessType;
    uint16_t _paramId;
    std::string getStrAccessType() const;
    void setAccessType(const AccessType& accessType);
    void setAccessType(const std::string& strAccessType);

    TocItem(const bitcraze::crazyflieLinkCpp::Packet& p_recv);
    TocItem();
    ~TocItem();
    friend std::ostream &operator<<(std::ostream& out, const TocItem& tocItem);
};

struct TocInfo
{
    uint16_t _numberOfElements;
    uint32_t _crc;
    TocInfo();
    TocInfo(const bitcraze::crazyflieLinkCpp::Packet& p_recv);
    ~TocInfo();
    friend std::ostream &operator<<(std::ostream& out, const TocInfo& tocInfo);
};

typedef std::pair<std::string,std::string> StrPair;

struct Toc
{
    std::map<StrPair,TocItem> _tocItems;
    TocInfo _tocInfo;
    TocItem getItem(const std::string& groupName, const std::string& paramName) const;
};
