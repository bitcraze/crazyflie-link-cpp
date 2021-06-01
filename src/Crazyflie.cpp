#include "Crazyflie.h"
using namespace bitcraze::crazyflieLinkCpp;

Crazyflie::Crazyflie(const std::string &uri) : _con(uri), _conWrapperParamRead(_con), _conWrapperParamWrite(_con), _conWrapperToc(_con), _conWrapperAppchannel(_con)
{
    _conWrapperToc.setPort(PARAM_PORT);
    _conWrapperParamRead.setPort(PARAM_PORT);
    _conWrapperParamWrite.setPort(PARAM_PORT);
    _conWrapperAppchannel.setPort(APPCHANNEL_PORT);

    _conWrapperToc.setChannel(TOC_CHANNEL);
    _conWrapperParamRead.setChannel(PARAM_READ_CHANNEL);
    _conWrapperParamWrite.setChannel(PARAM_WRITE_CHANNEL);
    _conWrapperAppchannel.setChannel(APP_CHANNEL);
}

void Crazyflie::sendAppChannelData(const void *data, const size_t &dataLen)
{
    _conWrapperAppchannel.sendData(data, dataLen);
}

std::vector<uint8_t> Crazyflie::recvAppChannelData()
{
    Packet p = _conWrapperAppchannel.recvFilteredData(0);

    std::vector<uint8_t> res;
    std::copy(p.payload(), p.payload() + p.payloadSize(), std::back_inserter(res));
    return res;
}

float Crazyflie::getFloatFromCrazyflie(uint16_t paramId) const
{
    float res = 0;

    _conWrapperParamRead.sendData(&paramId, sizeof(paramId));
    Packet p = _conWrapperParamRead.recvFilteredData(0);
    std::memcpy(&res, p.payload() + PAYLOAD_VALUE_BEGINING_INDEX, sizeof(res));

    return res;
}

uint32_t Crazyflie::getUIntFromCrazyflie(uint16_t paramId) const
{
    uint32_t res = 0;
    _conWrapperParamRead.sendData(&paramId, sizeof(paramId));

    Packet p = _conWrapperParamRead.recvFilteredData(0);

    std::memcpy(&res, p.payload() + PAYLOAD_VALUE_BEGINING_INDEX, p.payloadSize() - PAYLOAD_VALUE_BEGINING_INDEX);

    return res;
}

float Crazyflie::getFloatByName(const std::string &group, const std::string &name) const
{
    return getFloatFromCrazyflie(_toc.getItemId(group, name));
}

uint32_t Crazyflie::getUIntByName(const std::string &group, const std::string &name) const
{
    return getUIntFromCrazyflie(_toc.getItemId(group, name));
}

Crazyflie::~Crazyflie()
{
}

bool Crazyflie::setParamInCrazyflie(uint16_t paramId, float newValue)
{
    _conWrapperParamWrite.sendData(&paramId, sizeof(paramId), &newValue, sizeof(newValue));

    return true;
}

bool Crazyflie::setParamInCrazyflie(uint16_t paramId, uint32_t newValue, const size_t &valueSize)
{
    _conWrapperParamWrite.sendData(&paramId, sizeof(paramId), &newValue, valueSize);

    return true;
}

void Crazyflie::initToc()
{
    // ask for the toc info
    uint8_t cmd = CMD_TOC_INFO_V2;
    _conWrapperToc.sendData(&cmd, sizeof(cmd));
    Packet p_recv = _conWrapperToc.recvFilteredData(0);
    TocInfo cfTocInfo(p_recv);

    uint16_t num_of_elements = cfTocInfo._numberOfElements;

    for (uint16_t i = 0; i < num_of_elements; i++)
    {
        TocItem tocItem() ;
        _toc.insert(getTocItemFromCrazyflie(i));
    }
}

TocItem Crazyflie::getTocItemFromCrazyflie(uint16_t id) const
{
    uint8_t cmd = CMD_TOC_ITEM_V2;
    // ask for a param with the given id
    _conWrapperToc.sendData(&cmd, sizeof(uint8_t), &id, sizeof(id));
    Packet p_recv = _conWrapperToc.recvFilteredData(0);
    
    return TocItem(p_recv);
}

// //print the TOC with values!
void Crazyflie::printToc()
{
    auto tocItemsVector = _toc.getAllTocItems();

    for (TocItem tocItem : tocItemsVector)
    {
        std::cout << tocItem;
        if (to_string(tocItem._paramType).find("int") != std::string::npos)
            std::cout << getUIntFromCrazyflie(tocItem._paramId) << std::endl;
        else
            std::cout << getFloatFromCrazyflie(tocItem._paramId) << std::endl;
    }
    std::cout << "Printed " << tocItemsVector.size() << " items total" << std::endl;
}

bool Crazyflie::init()
{
    initToc();
    return true;
}

bool Crazyflie::setParamByName(const std::string& group, const std::string& name, float newValue)
{
    return setParamInCrazyflie(_toc.getItemId(group,name),newValue);
}
bool Crazyflie::setParamByName(const std::string& group, const std::string& name, uint32_t newValue, const size_t& valueSize)
{
    return setParamInCrazyflie(_toc.getItemId(group,name),newValue, valueSize);
}
bool Crazyflie::isParamFloat(const std::string& group, const std::string& name) const
{
    return to_string(_toc.getItem(group, name)._paramType) == "float";
}
