#include <iostream>
#include <thread>

#include "crazyflieCppLink/Connection.h"

int main()
{
    Connection con("radio://0/80/2M/E7E7E7E7E7");

    while (true) {
        Packet p = con.recv(/*blocking*/true);
        if (p.port() == 0 && p.channel() == 0) {
            std::string str((const char*)p.payload(), (size_t)p.payloadSize());
            std::cout << str;
            // std::cout << (int)p.size() << std::endl;
        }
    }

    return 0;
}