#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "ConnectionPlus.h"

#define PARAM_PORT 2
#define TOC_CHANNEL 0

#define CMD_TOC_ELEMENT 0 // original version: up to 255 entries
#define CMD_TOC_INFO 1    // original version: up to 255 entries
#define CMD_TOC_ITEM_V2 2 // version 2: up to 16k entries
#define CMD_TOC_INFO_V2 3 // version 2: up to 16k entries


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

    TocInfo(bitcraze::crazyflieLinkCpp::Packet& p_recv);

    friend std::ostream &operator<<(std::ostream &out, const TocInfo &tocInfo);
};

class Toc
{
public:
    Toc(const std::string &uri);

    void run();
    TocInfo getTocInfo();
    TocItem getItemFromToc(uint16_t id);

    std::vector<TocItem> getToc();
    void printToc();

private:
    ConnectionPlus _con;
};
