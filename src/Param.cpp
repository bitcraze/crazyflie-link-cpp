#include "Param.h"

Param::Param(bitcraze::crazyflieLinkCpp::Connection &con) : _conWrapper(con), _con(con), toc(con)
{
    _conWrapper.setPort(PARAM_PORT);
    _conWrapper.setChannel(PARAM_READ_CHANNEL);
}

float Param::getFloat(uint16_t paramId)
{
    float res;

    _conWrapper.sendData(paramId, sizeof(paramId));
    bitcraze::crazyflieLinkCpp::Packet p = _conWrapper.recvFilteredData(0);
    std::memcpy(&res, p.payload() + PAYLOAD_VALUE_BEGINING_INDEX, sizeof(res));

    return res;
}

uint32_t Param::getUInt(uint16_t paramId)
{
    uint32_t res;

    _conWrapper.sendData(paramId, sizeof(paramId));
    bitcraze::crazyflieLinkCpp::Packet p = _conWrapper.recvFilteredData(0);
    std::memcpy(&res, p.payload() + PAYLOAD_VALUE_BEGINING_INDEX, sizeof(res));

    return res;
}

float Param::getById(short paramId)
{
    auto tocItem = toc.getItemFromToc(paramId);
    std::string strType = toc.getAccessAndStrType(tocItem._paramType).second;
    if (strType.find("int") != std::string::npos)
    {
        return (float)getUInt(paramId);
    }
    else if ("float" == strType)
    {
        return getFloat(paramId);
    }
    else
    {
        std::cout << "Didn't find anything!" << std::endl;
        return NOT_FOUND;
    }
}

// MAYBE THERE'S A BETTER WAY TO DO IT ?
float Param::getByName(std::string group, std::string name)
{
    uint16_t numOfElements = toc.getTocInfo()._numberOfElements;

    for (uint16_t i = 0; i < numOfElements; i++)
    {
        auto tocItem = toc.getItemFromToc(i);
        if (tocItem._groupName == group && tocItem._paramName == name)
        {
            return getById(i);
        }
    }
    std::cout << "Didn't find anything!" << std::endl;
    return NOT_FOUND;
}

Toc Param::getToc()
{
    return toc;
}

Param::~Param()
{}