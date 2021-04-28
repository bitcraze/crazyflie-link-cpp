#include "crazyflieLinkCpp/Toc.h"
#include <bitset>

TocItem::TocItem(bitcraze::crazyflieLinkCpp::Packet &p_recv)
{
    _cmdNum = p_recv.payload()[0];
    _paramId = 0;
    memcpy(&_paramId, &p_recv.payload()[1], sizeof(_paramId));
    _paramType = p_recv.payload()[3];
    _groupName = (char *)(&p_recv.payload()[4]);
    _paramName = (char *)(&p_recv.payload()[4] + _groupName.length() + 1);
}

std::ostream &operator<<(std::ostream &out, const TocItem &tocItem)
{
    out << "cmdNum: " << (int)tocItem._cmdNum << std::endl;
    out << "paramId: " << (int)tocItem._paramId << std::endl;
    out << "paramType: " << (int)tocItem._paramType << std::endl;
    out << "groupName: " << tocItem._groupName << std::endl;
    out << "paramName: " << tocItem._paramName << std::endl;
    return out;
}

TocInfo::TocInfo(bitcraze::crazyflieLinkCpp::Packet &p_recv)
{
    memcpy(&_numberOfElements, &p_recv.payload()[1], sizeof(_numberOfElements));
    memcpy(&_crc, &p_recv.payload()[3], sizeof(_crc));
}

std::ostream &operator<<(std::ostream &out, const TocInfo &tocInfo)
{
    out << "numberOfElements: " << (int)tocInfo._numberOfElements << std::endl;
    out << "crc: " << (int)tocInfo._crc << std::endl;
    return out;
}

Toc::Toc(const std::string &uri)
    : _con(uri)
{
    _con.setPort(PARAM_PORT);
    _con.setChannel(TOC_CHANNEL);
}

void Toc::run()
{

    // empty receiver queue
    while (true)
    {
        bitcraze::crazyflieLinkCpp::Packet p = _con.recv(100);
        if (!p)
        {

            break;
        }
    }

    printToc();
}
TocInfo Toc::getTocInfo()
{
    //ask for the toc info

    _con.sendInt(CMD_TOC_INFO_V2);
    bitcraze::crazyflieLinkCpp::Packet p_recv = _con.recv(0);
    return TocInfo(p_recv);
}
TocItem Toc::getItemFromToc(uint16_t id)
{
    //ask for a param with the given id
    _con.sendInt(CMD_TOC_ITEM_V2, id);
    bitcraze::crazyflieLinkCpp::Packet p_recv = _con.recv(0);
    return TocItem(p_recv);
}

std::vector<TocItem> Toc::getToc()
{
    std::vector<TocItem> tocItems;

    uint16_t num_of_elements = getTocInfo()._numberOfElements;

    for (uint16_t i = 0; i < num_of_elements; i++)
    {
        tocItems.push_back(getItemFromToc(i));
    }

    return tocItems;
}
void Toc::printToc()
{
    auto tocItems = getToc();

    for (TocItem tocItem : tocItems)
    {
        // tocItem
        std::cout << tocItem._paramId << ": " << getStrType(tocItem._paramType) << "  " << tocItem._groupName << "." << tocItem._paramName << std::endl;
    }
}

std::string Toc::getStrType(uint8_t type)
{
    std::string accessType;
    // std::cout << std::bitset<8>(type).to_string()<< std::endl;
    // std::cout << (int)(type & ACCESS_TYPE_BYTE) << std::endl;

    if((bool)(type & ACCESS_TYPE_BYTE) == (bool)RO_ACCESS)
    {
        accessType = "RO";
        type = type & ~ACCESS_TYPE_BYTE;
    }
    else{
        accessType = "RW";
    }
    return accessType +":"+ types[type].first;
}
