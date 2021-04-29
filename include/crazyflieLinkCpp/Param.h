#pragma once
#include <iostream>
#include <sstream>
#include <cstring>

#include "ConnectionWrapper.h"

#define PARAM_READ_CHANNEL 1

class Param
{
private:
    ConnectionWrapper _conWrapper;
    bitcraze::crazyflieLinkCpp::Connection& _con;

public:
    Param(bitcraze::crazyflieLinkCpp::Connection &con);

    template <typename ReceiveType>
    ReceiveType getParam(uint16_t paramId)
    {
        ReceiveType res;
        _conWrapper.setPort(PARAM_PORT);
        _conWrapper.setChannel(PARAM_READ_CHANNEL);
        _conWrapper.sendObject<uint16_t>(paramId);

        bitcraze::crazyflieLinkCpp::Packet p = _con.recv(0);
        // std::cout << "payloadSize: " << p.payloadSize() << std::endl;

        if (p.payloadSize()-3 > sizeof(res))
        {
            std::stringstream errorMsg;
            errorMsg << "Size of package recieved (" << p.payloadSize()-3 << " bytes) is bigger than the size of the type given (" << sizeof(res) << " bytes)";
            throw(std::runtime_error(errorMsg.str()));
        }
        std::memcpy(&res, p.payload() + 3, sizeof(res));
        // std::cout << "pack: " << p << std::endl;
        // std::cout << "res: " << (int) res << std::endl;

        return res;
    }
};