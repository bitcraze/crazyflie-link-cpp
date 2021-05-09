#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>
#include <vector>

#include "crazyflieLinkCpp/ConnectionWrapper.h"

#define PARAM_PORT 2
#define TOC_CHANNEL 0
#define PARAM_READ_CHANNEL 1
#define PARAM_WRITE_CHANNEL 2

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
        ConnectionWrapper conWrapper(con);

        // empty receiver queue
        while (true)
        {
            Packet p = con.recv(100);
            if (!p)
            {
                break;
            }
        }

        conWrapper.setPort(PARAM_PORT);
        conWrapper.setChannel(PARAM_READ_CHANNEL);
        for (uint16_t i = 0; i < 100; i++)
        {
            conWrapper.sendData(i, sizeof(i));

            while (true)
            {
                Packet p = con.recv(100);

                if (!p)
                {
                    break;
                }
                std::cout << p << std::endl;
            }
        }
    }
    // TocInfo getTocInfo(Connection& con)
    // {
    //     //ask for the toc info
    //     sendInt(con, PARAM_PORT, TOC_CHANNEL, CMD_TOC_INFO_V2);
    //     Packet p_recv = con.recv(0);
    //     return TocInfo(p_recv);
    // }
    // TocItem getItemFromToc(Connection& con, uint16_t id)
    // {
    //     //ask for a param with the given id
    //     sendInt(con, PARAM_PORT, TOC_CHANNEL, CMD_TOC_ITEM_V2, id);
    //     Packet p_recv = con.recv(0);
    //     return TocItem (p_recv);
    // }

    // std::vector<TocItem> getToc(Connection& con)
    // {
    //     std::vector<TocItem> tocItems;

    //     uint16_t num_of_elements = getTocInfo(con)._numberOfElements;
    //     for (uint16_t i = 0; i < num_of_elements; i++)
    //     {
    //         tocItems.push_back(getItemFromToc(con, i));
    //     }
    //     return tocItems;
    // }
    // void printToc(Connection& con)
    // {
    //     auto tocItems = getToc(con);
    //     for(TocItem tocItem : tocItems)
    //     {
    //         // tocItem
    //         std::cout << tocItem._paramId << ": " << (int)tocItem._paramType << "  " << tocItem._groupName << "." << tocItem._paramName << std::endl;
    //     }
    // }

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