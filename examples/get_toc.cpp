#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>

#include "crazyflieLinkCpp/Connection.h"

using namespace bitcraze::crazyflieLinkCpp;

class GetToc
{
public:
    GetToc(const std::string& uri)
        : uri_(uri), _isFinished(false)
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

        Packet p;
        p.setPort(2);   // Param port
        p.setChannel(0); // toc channel
        p.
        p.setPayloadSize(sizeof(uint32_t));

        std::memcpy(p.payload(), &i, sizeof(i));

        con.send(p);
        _isFinished = true;
    }
public:
    bool _isFinished;

private:
    std::string uri_;

};

int main()
{
    std::vector<std::string> uris({"usb://0"});

    std::vector<GetToc> getTocs;
    getTocs.reserve(uris.size());
    std::vector<std::thread> threads;
    threads.reserve(uris.size());

    for (const auto& uri : uris) {
        getTocs.emplace_back(Benchmark(uri));
        threads.emplace_back(std::thread(&Benchmark::run, &getTocs.back()));
    }

    for (auto& t : threads) {
        t.join();
    }

    for (auto& b: getTocs) {
        std::cout << "Is Finished: " << b._isFinished << std::endl;
    }

    return 0;
}