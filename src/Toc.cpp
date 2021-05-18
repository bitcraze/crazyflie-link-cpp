#include "crazyflieLinkCpp/Toc.h"

#include <bitset>

using namespace bitcraze::crazyflieLinkCpp;

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

Toc::Toc(Connection &con) 
    : _conWrapper(con), _con(con)
{
    _conWrapper.setPort(PARAM_PORT);
    _conWrapper.setChannel(TOC_CHANNEL);
}

void Toc::run()
{   
    // empty receiver queue
    while (_con.recv(100)) {}
    printToc();
}

TocInfo Toc::getTocInfo()
{
    // ask for the toc info
    _conWrapper.sendData(CMD_TOC_INFO_V2, sizeof(uint8_t));
    bitcraze::crazyflieLinkCpp::Packet p_recv = _conWrapper.recvFilteredData(0);
    return TocInfo(p_recv);
}

TocItem Toc::getItemFromToc(uint16_t id)
{
    // ask for a param with the given id
    _conWrapper.sendData(CMD_TOC_ITEM_V2, sizeof(uint8_t), id, sizeof(id));
    bitcraze::crazyflieLinkCpp::Packet p_recv = _conWrapper.recvFilteredData(0);
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
        auto accessAndType = getAccessAndStrType(tocItem._paramType);
        std::cout << tocItem._paramId << ": " << accessTypeToStr(accessAndType.first) << ":" << accessAndType.second << "  " << tocItem._groupName << "." << tocItem._paramName << std::endl;
    }
}

std::string Toc::accessTypeToStr(int accessType)
{
    return RO_ACCESS == accessType ? "RO" : "RW";
}

std::pair<int, std::string> Toc::getAccessAndStrType(uint8_t type)
{
    int accessType;

    if ((bool)(type & ACCESS_TYPE_BYTE) == (bool)RO_ACCESS)
    {
        accessType = RO_ACCESS;
        type = type & ~ACCESS_TYPE_BYTE;
    }
    else
    {
        accessType = RW_ACCESS;
    }
    return std::pair<int, std::string>(accessType, types[type].first);
}
