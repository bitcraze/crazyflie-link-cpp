#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>

#include "crazyflieLinkCpp/Connection.h"

#define PARAM_PORT 2
#define TOC_CHANNEL 0

#define CMD_TOC_ELEMENT 0 // original version: up to 255 entries
#define CMD_TOC_INFO 1    // original version: up to 255 entries
#define CMD_TOC_ITEM_V2 2 // version 2: up to 16k entries
#define CMD_TOC_INFO_V2 3 // version 2: up to 16k entries

using namespace bitcraze::crazyflieLinkCpp;

class Benchmark
{
public:
    Benchmark(const std::string &uri)
        : uri_(uri)
    {
    }

    void run()
    {
        Connection con(uri_);

        // empty receiver queue
        while (true)
        {
            Packet p = con.recv(100);
            if (!p)
            {
                break;
            }
        }

        //ask for info
        sendInt(con, PARAM_PORT, TOC_CHANNEL, CMD_TOC_INFO_V2);

        Packet p_recv = con.recv(0);
        std::cout << p_recv << std::endl;
        uint16_t num_of_elements = 0;
        uint32_t crc = 0;
        memcpy(&num_of_elements, &p_recv.payload()[1], sizeof(num_of_elements));
        memcpy(&crc, &p_recv.payload()[3], sizeof(crc));
        std::cout <<"num of elements: "<< num_of_elements << std::endl;
        std::cout << "crc: "<<crc << std::endl;
        // p_recv = con.recv(0);
        // sendInt(2,0,0, con);
        // std::cout << p_recv << std::endl;
        // do
        // {
        //     p_recv = con.recv(0);
        // } while (p_recv.port() != 2);
        // std::cout << p_recv << std::endl;

        // sendInt(2,0,1, con);
        // do
        // {
        //     p_recv = con.recv(0);
        // std::cout << p_recv << std::endl;

        // } while (p_recv.port() != 2);

        // sendInt(2,0,1, con);
        // do
        // {
        //     p_recv = con.recv(0);
        //     if(p_recv.port()==0)
        //     std::cout << p_recv.payload() << std::endl;

        // } while (p_recv.port() != 2);
    }

    void sendInt(Connection &con, int port, int channel, int intigerToSend, int extraData = 0)
    {
        Packet p;
        p.setPort(port);
        p.setChannel(channel);
        if (extraData != 0)
        {
            p.setPayloadSize(sizeof(uint32_t) * 2);
            std::memcpy(p.payload(), &intigerToSend, sizeof(intigerToSend));
            std::memcpy(p.payload() + sizeof(uint32_t), &extraData, sizeof(extraData));
        }
        else
        {
            p.setPayloadSize(sizeof(uint32_t));
            std::memcpy(p.payload(), &intigerToSend, sizeof(intigerToSend));
        }

        con.send(p);
    }

public:
private:
    std::string uri_;
};

int main()
{
    std::vector<std::string> uris({"usb://0"});
    std::cout << "connecting to crazyflie via usb 0" << std::endl;

    std::vector<Benchmark> benchmarks;
    benchmarks.reserve(uris.size());
    std::vector<std::thread> threads;
    threads.reserve(uris.size());

    for (const auto &uri : uris)
    {
        benchmarks.emplace_back(Benchmark(uri));
        threads.emplace_back(std::thread(&Benchmark::run, &benchmarks.back()));
    }

    for (auto &t : threads)
    {
        t.join();
    }

    return 0;
}