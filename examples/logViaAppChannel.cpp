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

#define ACK_DELAY_MICRO_SEC 30

using namespace bitcraze::crazyflieLinkCpp;

int main()
{
    Crazyflie crazyflie("usb://0");

    crazyflie.init();

    crazyflie.setParamByName("usd", "logging", 0, 1);
    crazyflie.setParamByName("usd", "sendAppChannle", 1, 1);

    std::vector<uint8_t> result;
    uint32_t currMemAddress = 0;
    uint32_t dataSize = 0;
    std::ofstream outputFile("log.txt");
  
    auto start = std::chrono::steady_clock::now();

    do
    {
        result = crazyflie.recvAppChannelData();
        if(result.empty())
        {
            // std::cout << "Error Receiving from crazyflie" <<std::endl;
            break;
        }
        uint8_t packetCode = result[0];
        // std::cout << (int)packetCode << "-> ";
        std::vector<uint8_t> response;
        response.resize(6);
        unsigned int ackRequestMemAddress = 0;
        uint32_t crazyflieCurrMemAddress = 0;

        switch (packetCode)
        {
        case 0:
            std::copy_n(result.begin() + 1, sizeof(dataSize), (uint8_t *)&dataSize);
            // std::cout << "Data size: " << dataSize << std::endl;
            response[0] = 0;
            crazyflie.sendAppChannelData(&response[0], sizeof(uint8_t)); //size msg ack
            break;

        case 1:
            std::copy_n(result.begin() + 1, sizeof(crazyflieCurrMemAddress), (uint8_t *)&crazyflieCurrMemAddress);
            //send not all data recieved
            if(crazyflieCurrMemAddress != currMemAddress)
            {
                // std::cout << "Wrong Memory Address: " << (unsigned int)currMemAddress << std::endl;
                
                //response[0] = 2;
                //std::copy_n((uint8_t *)&currMemAddress, sizeof(currMemAddress), response.begin() + 1);
                //crazyflie.sendAppChannelData(response.data(), sizeof(uint8_t) + sizeof(uint32_t));
                // ackDelay += 5; 


                break;
            }
            currMemAddress += result.size() - sizeof(currMemAddress) - sizeof(uint8_t);
            // std::cout << "Current Memory Address: " << (unsigned int)currMemAddress << std::endl;
            outputFile.write(reinterpret_cast<char *>(&result[5]), result.size() - 5);

            break;

        case 2:
            std::copy_n(result.begin() + 1, sizeof(ackRequestMemAddress), (uint8_t *)&ackRequestMemAddress);
            std::this_thread::sleep_for(std::chrono::microseconds(ACK_DELAY_MICRO_SEC));
            // ackDelay -= 5;
            if (ackRequestMemAddress == currMemAddress)
            {
                // std::cout << "Ack Request Mem Address: " << ackRequestMemAddress << std::endl;

                response[0] = 0;
            }
            else
            {
                // std::cout << "Wrong Memory Address: " << (unsigned int)currMemAddress << std::endl;

                response[0] = 2;
            }
            std::copy_n((uint8_t *)&currMemAddress, sizeof(currMemAddress), response.begin() + 1);
            crazyflie.sendAppChannelData(response.data(), sizeof(uint8_t) + sizeof(uint32_t)); //data msg ack
            break;

        default:
            // std::cout << "Incorrect Msg type" << currMemAddress << std::endl;
            response[0] = 1;
            crazyflie.sendAppChannelData(response.data(), sizeof(uint8_t));

            break;
        }

    } while (currMemAddress < dataSize);
    outputFile.close();


    std::chrono::duration<double> delta =  std::chrono::steady_clock::now() - start;
    // std::cout << "Time [sec]: " << delta.count() << std::endl;
    // std::cout << "rate[bytes/sec] : " << dataSize/(delta.count()) <<std::endl;
    std::cout << dataSize/(delta.count()) <<std::endl;

    return 0;
}
