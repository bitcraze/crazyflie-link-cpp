// #include <iostream>
// #include <thread>
// #include <mutex>
// #include <condition_variable>
// #include <chrono>
// #include <sstream>
// #include <queue>
// #include <vector>

// #include "crazyflieLinkCpp/ConnectionWrapper.h"

// #define PARAM_PORT 2
// #define TOC_CHANNEL 0
// #define PARAM_READ_CHANNEL 1
// #define PARAM_WRITE_CHANNEL 2

// #define CMD_TOC_ELEMENT 0 // original version: up to 255 entries
// #define CMD_TOC_INFO 1    // original version: up to 255 entries
// #define CMD_TOC_ITEM_V2 2 // version 2: up to 16k entries
// #define CMD_TOC_INFO_V2 3 // version 2: up to 16k entries

// using namespace bitcraze::crazyflieLinkCpp;



// class Benchmark
// {
// public:
//     Benchmark(const std::string &uri)
//         : uri_(uri)
//     {
//     }

//     void run()
//     {
//         Connection con(uri_);
//         ConnectionWrapper conWrapper(con);

//         // empty receiver queue
//         while (true)
//         {
//             Packet p = con.recv(100);
//             if (!p)
//             {
//                 break;
//             }
//         }

//         conWrapper.setPort(PARAM_PORT);
//         conWrapper.setChannel(PARAM_READ_CHANNEL);
//         for (uint16_t i = 0; i < 100; i++)
//         {
//             conWrapper.sendData(i, sizeof(i));

//             while (true)
//             {
//                 Packet p = con.recv(100);

//                 if (!p)
//                 {
//                     break;
//                 }
//                 std::cout << p << std::endl;
//             }
//         }
//     }
//     // TocInfo getTocInfo(Connection& con)
//     // {
//     //     //ask for the toc info
//     //     sendInt(con, PARAM_PORT, TOC_CHANNEL, CMD_TOC_INFO_V2);
//     //     Packet p_recv = con.recv(0);
//     //     return TocInfo(p_recv);
//     // }
//     // TocItem getItemFromToc(Connection& con, uint16_t id)
//     // {
//     //     //ask for a param with the given id
//     //     sendInt(con, PARAM_PORT, TOC_CHANNEL, CMD_TOC_ITEM_V2, id);
//     //     Packet p_recv = con.recv(0);
//     //     return TocItem (p_recv);
//     // }

//     // std::vector<TocItem> getToc(Connection& con)
//     // {
//     //     std::vector<TocItem> tocItems;

//     //     uint16_t num_of_elements = getTocInfo(con)._numberOfElements;
//     //     for (uint16_t i = 0; i < num_of_elements; i++)
//     //     {
//     //         tocItems.push_back(getItemFromToc(con, i));
//     //     }
//     //     return tocItems;
//     // }
//     // void printToc(Connection& con)
//     // {
//     //     auto tocItems = getToc(con);
//     //     for(TocItem tocItem : tocItems)
//     //     {
//     //         // tocItem
//     //         std::cout << tocItem._paramId << ": " << (int)tocItem._paramType << "  " << tocItem._groupName << "." << tocItem._paramName << std::endl;
//     //     }
//     // }

// public:
// private:
//     std::string uri_;
// };

// int main()
// {
//     std::vector<std::string> uris({"usb://0"});
//     std::cout << "connecting to crazyflie via usb 0" << std::endl;

//     std::vector<Benchmark> benchmarks;
//     benchmarks.reserve(uris.size());
//     std::vector<std::thread> threads;
//     threads.reserve(uris.size());

//     for (const auto &uri : uris)
//     {
//         benchmarks.emplace_back(Benchmark(uri));
//         threads.emplace_back(std::thread(&Benchmark::run, &benchmarks.back()));
//     }

//     for (auto &t : threads)
//     {
//         t.join();
//     }

//     return 0;
// }
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
    std::vector<std::string> uris({"usb://0"});

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
