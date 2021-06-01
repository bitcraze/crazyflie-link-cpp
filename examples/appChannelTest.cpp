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
    // auto toc = crazyflie.getToc();

   

    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // for (auto element : toc)
    // {

    //     if (element._groupName == "usd" && element._paramName == "logging")
    //     {

    //         crazyflie.setParam(element._paramId, 0, 1);
    //         break;
    //     }
    // }

    // for (auto element : toc)
    // {

    //     if (element._groupName == "usd" && element._paramName == "sendAppChannle")
    //     {
    //         crazyflie.setParam(element._paramId, 1, 1);
    //         break;
    //     }
    // }

    // std::vector<uint8_t> res;
    // do
    // {

    //     res = crazyflie.recvAppChannelData();
    //     uint16_t sendData = 0;
    //     crazyflie.sendAppChannelData(&sendData,sizeof(sendData));
    //     for (auto element : res)
    //     {
    //         std::cout << (int)element;
    //     }

    //     std::cout << std::endl;

    // } while (res.size() > 0);

    return 0;
}