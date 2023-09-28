#include <iostream>
#include <thread>

#include "crazyflieLinkCpp/Connection.h"
#include "PacketUtils.hpp"

using namespace bitcraze::crazyflieLinkCpp;

void run1()
{
    Connection con("radio://0/80/2M/E7E7E7E7E7");

    const auto start = std::chrono::high_resolution_clock::now();
    while (true) {
        Packet p = con.recv(0);
        if (p.port() == 0 && p.channel() == 0) {
            std::string str((const char*)p.payload(), (size_t)p.payloadSize());
            std::cout << str;
        }

        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double, std::milli> elapsed = end - start;
        if (elapsed.count() > 5000) {
            break;
        }
    }
}

void run2()
{
    /* Creates a connection that will broadcast to all
     * crazyflies on channel 80. Note that this requires
     * updating the Crazyradio firmware from its factory version
     */
    Connection broadcastConnection("radiobroadcast://*/80/2M");

    // Requires high level commander enabled (param commander.enHighLevel == 1)
    std::cout << "Taking off..." << std::endl;
    broadcastConnection.send(PacketUtils::takeoffCommand(0.5f, 0.0f, 3.0f));
    std::this_thread::sleep_for(std::chrono::milliseconds(3250));

    std::cout << "Landing..." << std::endl;
    broadcastConnection.send(PacketUtils::landCommand(0.0f, 0.0f, 3.0f));
    std::this_thread::sleep_for(std::chrono::milliseconds(3250));

    std::cout << "Stopping..." << std::endl;
    broadcastConnection.send(PacketUtils::stopCommand());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    broadcastConnection.close();
    std::cout << "Done." << std::endl;
}

int main()
{
    std::thread t1(run1);
    std::thread t2(run2);

    t1.join();
    t2.join();

    return 0;
}