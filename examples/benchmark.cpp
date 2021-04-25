#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>

#include "crazyflieLinkCpp/Connection.h"

using namespace bitcraze::crazyflieLinkCpp;

class Benchmark
{
public:
    Benchmark(const std::string& uri)
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

        Packet p;
        // p.setPort(15);   // Link port
        p.setPort(2);   // param port
        p.setChannel(0); // toc channel
        p.setPayloadSize(sizeof(uint32_t));
        int i = 3;
        std::memcpy(p.payload(), &i, sizeof(i));
        con.send(p);
        Packet p_recv = con.recv(0);

        std::cout << p_recv << std::endl;
        int num_param = p_recv.payload()[1];
        std::cout << num_param << std::endl;
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

    for (const auto& uri : uris) {
        benchmarks.emplace_back(Benchmark(uri));
        threads.emplace_back(std::thread(&Benchmark::run, &benchmarks.back()));
    }

    for (auto& t : threads) {
        t.join();
    }

    // for (auto& b: benchmarks) {
    //     std::cout << "Latency: Min: " << b.latency_min * 1.0f << " ms; Avg: " << b.latency_avg * 1000.0f << " ms" << std::endl;
    //     std::cout << "Bandwidth: " << b.bandwidth << " packets/s" << std::endl;
    // }

    return 0;
}