#include "crazyflieLinkCpp/Param.h"

#define PAYLOAD_VALUE_BEGINING_INDEX 3
#define NOT_FOUND (-999)


Param::Param(bitcraze::crazyflieLinkCpp::Connection &con) : _conWrapper(con), _con(con)
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

int Param::getByName(Toc toc, std::string group, std::string name)
{
    uint16_t numOfElements = toc.getTocInfo()._numberOfElements;

    for (uint16_t i = 0; i < numOfElements; i++)
    {
        auto tocItem = toc.getItemFromToc(i);
        std::string strType = toc.getAccessAndStrType(tocItem._paramType).second;
        std::string strAccessType = toc.accessTypeToStr(toc.getAccessAndStrType(tocItem._paramType).first);
        if (tocItem._groupName == group && tocItem._paramName == name)
        {
            if (strType.find("int") != std::string::npos)
            {
                return getUInt(i);
            }
            else if ("float" == strType)
            {
                std::cout << "Wrong parameter type!\n";
                return NOT_FOUND;
            }
        }
    }
    std::cout << "Didn't find anything!" << std::endl;
    return NOT_FOUND;
}

Param::~Param()
{}