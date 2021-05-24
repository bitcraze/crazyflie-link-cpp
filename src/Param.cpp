#include "Param.h"

Param::Param(bitcraze::crazyflieLinkCpp::Connection &con) : _conWrapper(con), _con(con), _toc(con)
{
    _conWrapper.setPort(PARAM_PORT);
    
}

float Param::getFloat(uint16_t paramId)
{
    _conWrapper.setChannel(PARAM_READ_CHANNEL);
    float res = 0;

    _conWrapper.sendData(paramId, sizeof(paramId));
    bitcraze::crazyflieLinkCpp::Packet p = _conWrapper.recvFilteredData(0);
    std::memcpy(&res, p.payload() + PAYLOAD_VALUE_BEGINING_INDEX, sizeof(res));

    return res;
}

uint32_t Param::getUInt(uint16_t paramId)
{
    _conWrapper.setChannel(PARAM_READ_CHANNEL);

    uint32_t res = 0;

    _conWrapper.sendData(paramId, sizeof(paramId));

    bitcraze::crazyflieLinkCpp::Packet p = _conWrapper.recvFilteredData(0);
 
    std::memcpy(&res, p.payload() + PAYLOAD_VALUE_BEGINING_INDEX, p.payloadSize() - PAYLOAD_VALUE_BEGINING_INDEX);

    return res;
}

float Param::getFloatById(uint16_t paramId)
{
    auto tocItem = _toc.getItemFromToc(paramId);
    std::string strType = _toc.getAccessAndStrType(tocItem._paramType).second;

    if ("float" == strType)
    {
        return getFloat(paramId);
    }
    else
    {
        std::cout << "Didn't find anything!" << std::endl;
        return NOT_FOUND;
    }
}

uint32_t Param::getUIntById(uint16_t paramId)
{

    auto tocItem = _toc.getItemFromToc(paramId);
    std::string strType = _toc.getAccessAndStrType(tocItem._paramType).second;
    if (strType.find("int") != std::string::npos)
    {
        return getUInt(paramId);
    }
    else
    {
        std::cout << "Didn't find anything!" << std::endl;
        return NOT_FOUND;
    }
}

float Param::getFloatByName(std::string group, std::string name)
{
    uint16_t numOfElements = _toc.getTocInfo()._numberOfElements;

    for (uint16_t i = 0; i < numOfElements; i++)
    {
        auto tocItem = _toc.getItemFromToc(i);

        if (tocItem._groupName == group && tocItem._paramName == name)
        {

            return getFloatById(tocItem._paramId);
        }
    }
    std::cout << "Didn't find anything!" << std::endl;
    return NOT_FOUND;
}

uint32_t Param::getUIntByName(std::string group, std::string name)
{
    uint16_t numOfElements = _toc.getTocInfo()._numberOfElements;

    for (uint16_t i = 0; i < numOfElements; i++)
    {
        auto tocItem = _toc.getItemFromToc(i);

        if (tocItem._groupName == group && tocItem._paramName == name)
        {
            return getUIntById(tocItem._paramId);
        }
    }
    std::cout << "Didn't find anything!" << std::endl;
    return NOT_FOUND;
}

Toc Param::getToc()
{
    return _toc;
}

Param::~Param()
{
}

bool Param::setParam(uint16_t paramId, float newValue) 
{
    _conWrapper.setChannel(PARAM_WRITE_CHANNEL);

    _conWrapper.sendData(paramId, sizeof(paramId), newValue, sizeof(newValue));

    return true;
}

bool Param::setParam(uint16_t paramId, uint32_t newValue, size_t valueSize) 
{
    _conWrapper.setChannel(PARAM_WRITE_CHANNEL);

    _conWrapper.sendData(paramId, sizeof(paramId), newValue, valueSize);

    return true;
}
