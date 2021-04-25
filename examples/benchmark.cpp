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

        sendInt(2,0,0, con);

        sendInt(2,0,3, con);
        
        Packet p_recv = con.recv(0);
        std::cout << p_recv << std::endl;
        int num_param = p_recv.payload()[1];
        std::cout << num_param << std::endl;

        p_recv = con.recv(0);
        sendInt(2,0,0, con);
        std::cout << p_recv << std::endl;
        do
        {
            p_recv = con.recv(0);
        } while (p_recv.port() != 2);
        std::cout << p_recv << std::endl;
    

        sendInt(2,0,1, con);
        do
        {
            p_recv = con.recv(0);
        std::cout << p_recv << std::endl;

        } while (p_recv.port() != 2);

        sendInt(2,0,1, con);
        do
        {
            p_recv = con.recv(0);
            if(p_recv.port()==0)
            std::cout << p_recv.payload() << std::endl;

        } while (p_recv.port() != 2);
        
    }
        

    void sendInt(int port, int channel, int intigerToSend, Connection& con)
    {
        Packet p;
        p.setPort(port);   
        p.setChannel(channel); 
        p.setPayloadSize(sizeof(uint32_t));
        std::memcpy(p.payload(), &intigerToSend, sizeof(intigerToSend));

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

    for (const auto& uri : uris) {
        benchmarks.emplace_back(Benchmark(uri));
        threads.emplace_back(std::thread(&Benchmark::run, &benchmarks.back()));
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}