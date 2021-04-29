#include "crazyflieLinkCpp/ConnectionPlus.h"

using namespace bitcraze::crazyflieLinkCpp;


ConnectionPlus::ConnectionPlus(std::string uri) : Connection(uri)
{
}

ConnectionPlus::~ConnectionPlus()
{
}

void ConnectionPlus::sendInt(uint8_t intigerToSend)
{
    _packet.setPayloadSize(sizeof(intigerToSend));
    std::memcpy(_packet.payload(), &intigerToSend, sizeof(intigerToSend));

    this->send(_packet);
}

void ConnectionPlus::sendInt(uint8_t intigerToSend, uint16_t extraData)
{

    _packet.setPayloadSize(sizeof(intigerToSend) + sizeof(extraData));
    std::memcpy(_packet.payload(), &intigerToSend, sizeof(intigerToSend));
    std::memcpy(_packet.payload() + sizeof(intigerToSend), &extraData, sizeof(extraData));

    this->send(_packet);
}

void ConnectionPlus::setPort(int port)
{
    _packet.setPort(port);
}
void ConnectionPlus::setChannel(int channel)
{
    _packet.setChannel(channel);
}
void ConnectionPlus::sendInt(uint8_t intigerToSend, int port, int channel)
{
    this->setPort(port);
    this->setChannel(channel);
    this->sendInt(intigerToSend);
}
void ConnectionPlus::sendInt(uint8_t intigerToSend, uint16_t extraData, int port, int channel)
{

    this->setPort(port);
    this->setChannel(channel);
    this->sendInt(intigerToSend, extraData);
}
