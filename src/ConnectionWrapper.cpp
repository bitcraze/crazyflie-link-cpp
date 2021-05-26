#ifndef __CONNECTIONWRAPPER_H__
#define __CONNECTIONWRAPPER_H__

#include "ConnectionWrapper.h"
#include <functional>

using namespace bitcraze::crazyflieLinkCpp;

ConnectionWrapper& ConnectionWrapper::operator=(bitcraze::crazyflieLinkCpp::Connection& con)
{
    _conPtr = &con;
    return *this;
}


ConnectionWrapper::ConnectionWrapper(Connection &con) : _conPtr(&con)
{
}

ConnectionWrapper::~ConnectionWrapper()
{
}

bool isBigEndian()
{
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1; 
}

void ConnectionWrapper::sendData(const void* data1, const size_t& data1_len, const void* data2, const size_t& data2_len) const
{
    Packet p;
    p.setChannel(_channel);
    p.setPort(_port);
    p.setPayloadSize(data1_len + data2_len);

    if (IS_BIG_ENDIAN)
    {
        std::reverse_copy((uint8_t*)data1 + data1_len, (uint8_t*)data1, p.payload());
        if (data2_len > 0)
        {
            std::reverse_copy((uint8_t*)data2 + data2_len, (uint8_t*)data2, p.payload() + data1_len);
        }
    }
    else
    {
        std::copy( (uint8_t*)data1, (uint8_t*)data1 + data1_len, p.payload());
        if (data2_len > 0)
        {
            std::copy( (uint8_t*)data2, (uint8_t*)data2 + data2_len, p.payload() + data1_len);
        }
    }
    _conPtr->send(p);
}


Packet ConnectionWrapper::recvFilteredData(int timeout, int port, int channel) const
{
    while (true)
    {
        Packet p = _conPtr->recv(timeout);
        if ((p.channel() == channel && p.port() == port) || !p)
            return p;
    }
}

Packet ConnectionWrapper::recvFilteredData(int timeout) const
{
    return this->recvFilteredData(timeout, _port, _channel);
}

Connection &ConnectionWrapper::getConnection()
{
    return *_conPtr;
}

void ConnectionWrapper::setPort(int port)
{
    _port = port;
}

void ConnectionWrapper::setChannel(int channel)
{
    _channel = channel;
}
#endif // __CONNECTIONWRAPPER_H__