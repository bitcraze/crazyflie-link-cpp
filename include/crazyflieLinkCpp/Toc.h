#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include "ConnectionPlus.h"

#define PARAM_PORT 2
#define TOC_CHANNEL 0

#define CMD_TOC_ELEMENT 0 // original version: up to 255 entries
#define CMD_TOC_INFO 1    // original version: up to 255 entries
#define CMD_TOC_ITEM_V2 2 // version 2: up to 16k entries
#define CMD_TOC_INFO_V2 3 // version 2: up to 16k entries

#define RW_ACCESS 0
#define RO_ACCESS 1
#define ACCESS_TYPE_BYTE 64



struct TocItem
{
    std::string _groupName;
    std::string _paramName;
    uint8_t _cmdNum;
    uint8_t _paramType;
    uint16_t _paramId;

    TocItem(bitcraze::crazyflieLinkCpp::Packet &p_recv);
    friend std::ostream &operator<<(std::ostream &out, const TocItem &tocItem);
};

struct TocInfo
{
    uint16_t _numberOfElements;
    uint32_t _crc;

    TocInfo(bitcraze::crazyflieLinkCpp::Packet &p_recv);

    friend std::ostream &operator<<(std::ostream &out, const TocInfo &tocInfo);
};

class Toc
{
    
    
public:
    Toc(const std::string &uri);

    void run();
    TocInfo getTocInfo();
    TocItem getItemFromToc(uint16_t id);
    std::string getStrType(uint8_t type);

    std::vector<TocItem> getToc();
    void printToc();

    typedef std::pair<std::string, std::string> StrPair;
    typedef std::pair<uint8_t, StrPair> TypeMapPair;
    std::map<uint8_t, StrPair>
    types = {TypeMapPair(0x08, StrPair({"uint8_t", "<B"})),
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

private:
    ConnectionPlus _con;
};
