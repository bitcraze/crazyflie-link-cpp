#include "crazyflieLinkCpp/ConnectionWrapper.h"

using namespace bitcraze::crazyflieLinkCpp;

ConnectionWrapper::ConnectionWrapper(Connection &con) : _con(con)
{
}

ConnectionWrapper::~ConnectionWrapper()
{
}

void ConnectionWrapper::sendData(uint32_t data1, size_t data1_len, uint32_t data2, size_t data2_len)
{
    _packet.setPayloadSize(data1_len + data2_len );
    std::memcpy(_packet.payload(), &data1, data1_len);

    if (data2_len != 0)
    {
        std::memcpy(_packet.payload()+ data1_len, &data2 , data2_len);
    }
    std::cout << _packet << std::endl;
    _con.send(_packet);
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