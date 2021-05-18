#include "crazyflieLinkCpp/ConnectionWrapper.h"

using namespace bitcraze::crazyflieLinkCpp;

ConnectionWrapper::ConnectionWrapper(Connection &con) : _con(con)
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

void ConnectionWrapper::sendData(uint32_t data1, size_t data1_len, uint32_t data2, size_t data2_len)
{
    _packet.setPayloadSize(data1_len + data2_len);

    if (IS_BIG_ENDIAN)
    {
        std::reverse_copy((uint8_t*)&data1 + data1_len, (uint8_t*)&data1, _packet.payload());
        if (data2_len != 0)
        {
            std::reverse_copy((uint8_t*)&data2 + data2_len, (uint8_t*)&data2, _packet.payload() + data1_len);
        }
    }
    else
    {
        std::copy( (uint8_t*)&data1, (uint8_t*)&data1 + data1_len, _packet.payload());
        if (data2_len != 0)
        {
            std::copy( (uint8_t*)&data2, (uint8_t*)&data2 + data2_len, _packet.payload() + data1_len);
        }
    }
    _con.send(_packet);
}


bitcraze::crazyflieLinkCpp::Packet ConnectionWrapper::recvFilteredData(int timeout, int port, int channel)
{
    while (true)
    {
        bitcraze::crazyflieLinkCpp::Packet p = _con.recv(timeout);
        if (p.channel() == channel && p.port() == port || !p)
            return p;
    }
}

bitcraze::crazyflieLinkCpp::Packet ConnectionWrapper::recvFilteredData(int timeout)
{
    return this->recvFilteredData(timeout, _packet.port(), _packet.channel());
}

Connection &ConnectionWrapper::getConnection()
{
    return _con;
}

void ConnectionWrapper::setPort(int port)
{
    _packet.setPort(port);
}

void ConnectionWrapper::setChannel(int channel)
{
    _packet.setChannel(channel);
}