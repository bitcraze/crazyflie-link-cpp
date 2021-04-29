#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>
#include <vector>

#include "crazyflieLinkCpp/Toc.h"


using namespace bitcraze::crazyflieLinkCpp;


// class Test
// {
// public:
//     Test(const std::string &uri)
//         : _toc(uri)
//     {
//     }

//     void run()
//     {
//         _toc.run();
//     }
   

// public:
// private:
//     Toc _toc;
// };

int main()
{
    Connection con("usb://0");
    Toc toc(con);
    toc.run();
    // std::vector<std::string> uris({"usb://0"});
    // std::cout << "connecting to crazyflie via usb 0" << std::endl;

    // std::vector<Test> benchmarks;
    // benchmarks.reserve(uris.size());
    // std::vector<std::thread> threads;
    // threads.reserve(uris.size());

    // for (const auto &uri : uris)
    // {
    //     benchmarks.emplace_back(Test(uri));
    //     threads.emplace_back(std::thread(&Test::run, &benchmarks.back()));
    // }

    // for (auto &t : threads)
    // {
    //     t.join();
    // }

    return 0;
}