#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>
#include <vector>
#include <thread>
#include "Crazyflie.h"

using namespace bitcraze::crazyflieLinkCpp;

int main()
{
    Crazyflie crazyflie("usb://0");
    auto toc = crazyflie.getToc();

        std::cout << "pass" << std::endl;

    for (auto element : toc)
    {
        if (element._groupName == "usd" && element._paramName == "sendAppChannel")
        {
            crazyflie.setParam(element._paramId, 0, 1);
            break;
        }
    }
        std::cout << "pass" << std::endl;

    for (auto element : toc)
    {
        if (element._groupName == "usd" && element._paramName == "logging")
        {
            crazyflie.setParam(element._paramId, 1, 1);
            break;
        }
    }
        std::cout << "pass" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    for (auto element : toc)
    {

        if (element._groupName == "usd" && element._paramName == "logging")
        {

            crazyflie.setParam(element._paramId, 0, 1);
            break;
        }
    }
        std::cout << "pass" << std::endl;

    for (auto element : toc)
    {
        if (element._groupName == "usd" && element._paramName == "sendAppChannel")
        {
            crazyflie.setParam(element._paramId, 1, 1);
            break;
        }
    }
        std::cout << "pass" << std::endl;

    std::vector<uint8_t> res;
    do
    {
        std::cout << "pass1" << std::endl;

        res = crazyflie.recvAppChannelData();
        std::cout << "pass1" << std::endl;

        for (auto element : res)
        {
            std::cout << element;
        }
        std::cout << "pass1" << std::endl;

        std::cout << std::endl;

    } while (res.size() > 0);
        std::cout << "pass" << std::endl;

    return 0;
}