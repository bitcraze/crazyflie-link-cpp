#include <iostream>
#include <thread>

#include "crazyflieLinkCpp/Connection.h"

using namespace bitcraze::crazyflieLinkCpp;

int main()
{
    Connection con("radio://0/80/2M/E7E7E7E7E7");

    while (true) {
        Packet p = con.recv(0);
        if (p.port() == 0 && p.channel() == 0) {
            std::string str((const char*)p.payload(), (size_t)p.payloadSize());
            std::cout << str;
        }
    }

    return 0;
}