#pragma once
#include <iostream>

#include "ConnectionPlus.h"

#define PARAM_READ_CHANNEL 1

class Param
{
private:
    ConnectionPlus _con;

public:
    Param(const std::string &uri);

    template <typename ReceiveType>
    ReceiveType getParam(uint16_t paramId)
    {
        ReceiveType res;
        con.setPort(PARAM_PORT);
        con.setChannel(PARAM_READ_CHANNEL);
        con.sendObject<uint16_t>(paramId);

        bitcraze::crazyflieLinkCpp::Packet p = con.recv(0);

        if (p.payloadSize() > sizeof(ReceiveType))
            throw(std::runtime_error("Size of package recieved (" + p.payloadSize() + " bytes) is bigger than the size of the type given (" + sizeof(ReceiveType) + " bytes)"));

        std::memcopy(&res, p.payload(), sizeof(res));

        return res;
    }
};