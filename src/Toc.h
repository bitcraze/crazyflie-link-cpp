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

struct TocInfo
{
    uint16_t _numberOfElements;
    uint32_t _crc;

    TocInfo(Packet& p_recv)
    {
        memcpy(&_numberOfElements, &p_recv.payload()[1], sizeof(_numberOfElements));
        memcpy(&_crc, &p_recv.payload()[3], sizeof(_crc));
    }

    friend std::ostream &operator<<(std::ostream &out, const TocInfo &tocInfo)
    {
        out << "numberOfElements: " << (int)tocInfo._numberOfElements << std::endl;
        out << "crc: " << (int)tocInfo._crc << std::endl;
        return out;
    }
};

class Toc
{
public:
    Toc(const std::string &uri)
        : _con(uri)
    {
    }

    void run()
    {

        // empty receiver queue
        while (true)
        {
            Packet p = _con.recv(100);
            if (!p)
            {
                break;
            }
        }
        
        printToc(_con);
      
        
    }
    TocInfo getTocInfo()
    {
        //ask for the toc info
        sendInt(_con, PARAM_PORT, TOC_CHANNEL, CMD_TOC_INFO_V2);
        Packet p_recv = _con.recv(0);
        return TocInfo(p_recv);
    }
    TocItem getItemFromToc(uint16_t id)
    {
        //ask for a param with the given id
        sendInt(_con, PARAM_PORT, TOC_CHANNEL, CMD_TOC_ITEM_V2, id);
        Packet p_recv = _con.recv(0);
        return TocItem (p_recv);
    }

    std::vector<TocItem> getToc()
    {
        std::vector<TocItem> tocItems;

        uint16_t num_of_elements = getTocInfo()._numberOfElements;
        for (uint16_t i = 0; i < num_of_elements; i++)
        {
            tocItems.push_back(getItemFromToc(i));
        }
        return tocItems;
    }
    void printToc()
    {
        auto tocItems = getToc();
        for(TocItem tocItem : tocItems)
        {
            // tocItem
            std::cout << tocItem._paramId << ": " << (int)tocItem._paramType << "  " << tocItem._groupName << "." << tocItem._paramName << std::endl;
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

private:
    Connection _con;
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