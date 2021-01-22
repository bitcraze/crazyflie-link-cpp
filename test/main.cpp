#include <iostream>
#include <thread>

#include "Connection.h"
#include "Crazyradio.h"

int main()
{
    // scanning
    const auto uris = Connection::scan("");

    for (const auto uri : uris) {
        std::cout << uri << std::endl;
    }

    // connection

    Connection con("radio://0/80/2M/E7E7E7E7E7");

    while (true) {
        Packet p = con.recv(false);
        if (p && p.port() == 0 && p.channel() == 0) {
            std::cout << p.data();
        }
    }
    // std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}