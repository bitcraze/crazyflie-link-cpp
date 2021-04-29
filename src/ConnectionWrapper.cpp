#include "crazyflieLinkCpp/ConnectionWrapper.h"

using namespace bitcraze::crazyflieLinkCpp;


ConnectionWrapper::ConnectionWrapper(Connection& con) : _con(con)
{
}

ConnectionWrapper::~ConnectionWrapper()
{
}

void ConnectionWrapper::sendInt(uint8_t intigerToSend)
{
    _packet.setPayloadSize(sizeof(intigerToSend));
    std::memcpy(_packet.payload(), &intigerToSend, sizeof(intigerToSend));
    std::cout << "sending: " << _packet << std::endl;

    _con.send(_packet);
}

void ConnectionWrapper::sendInt(uint8_t intigerToSend, uint16_t extraData)
{

    _packet.setPayloadSize(sizeof(intigerToSend) + sizeof(extraData));
    std::memcpy(_packet.payload(), &intigerToSend, sizeof(intigerToSend));
    std::memcpy(_packet.payload() + sizeof(intigerToSend), &extraData, sizeof(extraData));

    _con.send(_packet);
}

void ConnectionWrapper::setPort(int port)
{
    _packet.setPort(port);
}
void ConnectionWrapper::setChannel(int channel)
{
    _packet.setChannel(channel);
}
void ConnectionWrapper::sendInt(uint8_t intigerToSend, int port, int channel)
{
    this->setPort(port);
    this->setChannel(channel);
    this->sendInt(intigerToSend);
}
void ConnectionWrapper::sendInt(uint8_t intigerToSend, uint16_t extraData, int port, int channel)
{

    this->setPort(port);
    this->setChannel(channel);
    this->sendInt(intigerToSend, extraData);
}

Connection& ConnectionWrapper::getConnection()
{
    return _con;
}