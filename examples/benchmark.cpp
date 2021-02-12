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
        p.setPort(15);   // Link port
        p.setChannel(0); // echo channel
        p.setPayloadSize(sizeof(uint32_t));
        
        // Latency experiment
        double latency_sum = 0.0;
        latency_min = std::numeric_limits<double>::max();
        const size_t count = 1000;
        for (uint32_t i = 0; i < count; ++i)
        {
            std::memcpy(p.payload(), &i, sizeof(i));
            auto start = std::chrono::steady_clock::now();
            con.send(p);
            Packet p_recv = con.recv(0);
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end - start;

            uint32_t recv_i;
            std::memcpy(&recv_i, p_recv.payload(), sizeof(recv_i));
            if (p_recv.payloadSize() != p.payloadSize() || i != recv_i)
            {
                std::stringstream sstr;
                sstr << "Latency: Sent " << i << " but received " << recv_i << "! (" << p_recv << ")";
                throw std::runtime_error(sstr.str());
            }
            latency_sum += diff.count();
            latency_min = std::min(diff.count(), latency_min);
            // std::cout << p_recv << " " << diff.count() << std::endl;
        }
        latency_avg = latency_sum / (double)count;

        // Bandwidth experiment
        auto start = std::chrono::steady_clock::now();
        for (uint32_t i = 0; i < count; ++i)
        {
            std::memcpy(p.payload(), &i, sizeof(i));
            con.send(p);
        }
        Packet p_old;
        for (uint32_t i = 0; i < count; ++i)
        {
            Packet p_recv = con.recv(0);
            uint32_t recv_i;
            std::memcpy(&recv_i, p_recv.payload(), sizeof(recv_i));
            if (p_recv.payloadSize() != p.payloadSize() || i != recv_i)
            {
                std::stringstream sstr;
                sstr << "Bandwith: Sent " << i << " but received " << recv_i << "! (" << p_recv << ")";

                // DBG
                sstr << std::endl;
                sstr << " previous: " << p_old << std::endl;
                sstr << " current: " << p_recv << std::endl;
                sstr << " new/future ";
                for (size_t k = 0; k < 10; ++k) {
                    Packet p_recv2 = con.recv(0);
                    sstr << p_recv2 << std::endl;
                }

                // DBG


                throw std::runtime_error(sstr.str());
            }
            p_old = p_recv;
        }
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = end - start;
        double total_duration = diff.count();
        bandwidth = count / total_duration;
    }


public:
    double latency_avg;
    double latency_min;
    double bandwidth;

private:
    std::string uri_;

};

int main()
{
    // std::vector<std::string> uris({"radio://*/80/2M/E7E7E7E7E7", "radio://*/90/2M/E7E7E7E7E7"});
    std::vector<std::string> uris({"radio://*/80/2M/E7E7E7E7E7"});

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

    for (auto& b: benchmarks) {
        std::cout << "Latency: Min: " << b.latency_min * 1000.0f << " ms; Avg: " << b.latency_avg * 1000.0f << " ms" << std::endl;
        std::cout << "Bandwidth: " << b.bandwidth << " packets/s" << std::endl;
    }

    return 0;
}