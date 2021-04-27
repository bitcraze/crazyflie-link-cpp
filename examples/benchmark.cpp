#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>
#include <vector>

#include "crazyflieLinkCpp/Connection.h"

#define PARAM_PORT 2
#define TOC_CHANNEL 0

#define CMD_TOC_ELEMENT 0 // original version: up to 255 entries
#define CMD_TOC_INFO 1    // original version: up to 255 entries
#define CMD_TOC_ITEM_V2 2 // version 2: up to 16k entries
#define CMD_TOC_INFO_V2 3 // version 2: up to 16k entries

using namespace bitcraze::crazyflieLinkCpp;

struct TocItem
{
    std::string _groupName;
    std::string _paramName;
    uint8_t _cmdNum;
    uint8_t _paramType;
    uint16_t _paramId;

    TocItem(Packet &p_recv)
    {
        _cmdNum = p_recv.payload()[0];
        _paramId = 0;
        memcpy(&_paramId, &p_recv.payload()[1], sizeof(_paramId));
        _paramType = p_recv.payload()[3];
        _groupName = (char *)(&p_recv.payload()[4]);
        _paramName = (char *)(&p_recv.payload()[4] + _groupName.length() + 1);
    }
    friend std::ostream &operator<<(std::ostream &out, const TocItem &tocItem)
    {
        out << "cmdNum: " << (int)tocItem._cmdNum << std::endl;
        out << "paramId: " << (int)tocItem._paramId << std::endl;
        out << "paramType: " << (int)tocItem._paramType << std::endl;
        out << "groupName: " << tocItem._groupName << std::endl;
        out << "paramName: " << tocItem._paramName << std::endl;
        return out;
    }
};

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
        std::cout << "num of elements: " << num_of_elements << std::endl;
        std::cout << "crc: " << crc << std::endl;

        //ask for item

        for (uint8_t i = 0; i < num_of_elements; i++)
        {
            sendInt(con, PARAM_PORT, TOC_CHANNEL, CMD_TOC_ITEM_V2, i);
            p_recv = con.recv(0);
            TocItem tocItem(p_recv);
            std::cout << tocItem._paramId << ": " << tocItem._groupName << "." << tocItem._paramName << std::endl;
        }
        printToc(con);
    }

    TocItem getItemFromToc(Connection& con, uint16_t id)
    {
        sendInt(con, PARAM_PORT, TOC_CHANNEL, CMD_TOC_ITEM_V2, id);
        Packet p_recv = con.recv(0);
        TocItem tocItem(p_recv);
        return tocItem;
    }

    std::vector<TocItem> getToc(Connection& con)
    {
        std::vector<TocItem> tocItems;
        //ask for the toc info
        sendInt(con, PARAM_PORT, TOC_CHANNEL, CMD_TOC_INFO_V2);

        Packet p_recv = con.recv(0);
        uint16_t num_of_elements = 0;
        memcpy(&num_of_elements, &p_recv.payload()[1], sizeof(num_of_elements));

        for (uint8_t i = 0; i < num_of_elements; i++)
        {
            sendInt(con, PARAM_PORT, TOC_CHANNEL, CMD_TOC_ITEM_V2, i);
            p_recv = con.recv(0);

            tocItems.push_back(p_recv);

        }
        return tocItems;
    }
    void printToc(Connection& con)
    {
        auto tocItems = getToc(con);
        for(TocItem tocItem : tocItems)
        {
            std::cout << tocItem._paramId << ": " << tocItem._groupName << "." << tocItem._paramName << std::endl;
        }
    }

    void sendInt(Connection &con, int port, int channel, uint8_t intigerToSend)
    {
        Packet p;
        p.setPort(port);
        p.setChannel(channel);

        p.setPayloadSize(sizeof(intigerToSend));
        std::memcpy(p.payload(), &intigerToSend, sizeof(intigerToSend));

        con.send(p);
    }

    void sendInt(Connection &con, int port, int channel, uint8_t intigerToSend, uint8_t extraData)
    {
        Packet p;
        p.setPort(port);
        p.setChannel(channel);

        p.setPayloadSize(sizeof(intigerToSend) + sizeof(extraData));
        std::memcpy(p.payload(), &intigerToSend, sizeof(intigerToSend));
        std::memcpy(p.payload() + sizeof(intigerToSend), &extraData, sizeof(extraData));

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