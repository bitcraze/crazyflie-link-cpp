#include "crazyflieLinkCpp/Param.h"

#define PAYLOAD_VALUE_BEGINING_INDEX 3


Param::Param(bitcraze::crazyflieLinkCpp::Connection &con) : _conWrapper(con), _con(con)
{
    _conWrapper.setPort(PARAM_PORT);
    _conWrapper.setChannel(PARAM_READ_CHANNEL);
}

float Param::getFloat(uint16_t paramId)
{
    float res;

    _conWrapper.sendData(paramId, sizeof(paramId));

    bitcraze::crazyflieLinkCpp::Packet p = _con.recv(0);

    std::memcpy(&res, p.payload() + PAYLOAD_VALUE_BEGINING_INDEX, sizeof(res));

    return res;
}

uint32_t Param::getUInt(uint16_t paramId)
{
    uint32_t res;

    _conWrapper.sendData(paramId, sizeof(paramId));

    bitcraze::crazyflieLinkCpp::Packet p = _con.recv(0);

    std::memcpy(&res, p.payload() + PAYLOAD_VALUE_BEGINING_INDEX, sizeof(res));

    return res;
}
    
Param::~Param()
{}