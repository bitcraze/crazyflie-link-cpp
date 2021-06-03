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



int main()
{
    Crazyflie crazyflie("usb://0");

    crazyflie.init();

    LoggingMsg msgRecieved;
    uint32_t currMemAddress = 0;
    std::queue<uint32_t> lostMemAddresses;
    uint32_t dataSize = 0;
    std::ofstream outputFile("log.txt", std::ifstream::binary | std::ios::out | std::ios::app);
    auto start = std::chrono::steady_clock::now();
    size_t fileSize = 0;
    do
    {
        uint8_t bytesRecieved = crazyflie.recvAppChannelData(&msgRecieved, sizeof(msgRecieved));
        if (0 == bytesRecieved)
        {
            std::cout << "Error Receiving from crazyflie" << std::endl;
            break;
        }

        std::cout << (int)msgRecieved._msgType << "-> ";
        LoggingMsg response;
        size_t responseSize = 0;
        unsigned int ackRequestMemAddress = 0;
        uint32_t crazyflieCurrMemAddress = 0;
        // std::cout << "bytes received: " << (int)bytesRecieved << std::endl;
        uint8_t dataRecivedSize = bytesRecieved - sizeof(response._header);

        if (crazyflie.isRunning() == false)
        {
            response._msgType = 3;
            crazyflie.sendAppChannelData(&response, sizeof(uint8_t));
            return 0;
        }
        switch (msgRecieved._msgType)
        {
        case 0:
            dataSize = msgRecieved._sequence;
            std::cout << "Data size: " << dataSize << std::endl;
            response._msgType = 0;
            responseSize = sizeof(uint8_t);
            break;

        case 1:
            crazyflieCurrMemAddress = msgRecieved._sequence;
            outputFile.seekp(crazyflieCurrMemAddress);
            outputFile.write((const char *)msgRecieved._data, dataRecivedSize);

            if (crazyflieCurrMemAddress != currMemAddress)
            {
                lostMemAddresses.push(currMemAddress);
                std::cout << "Lost Address: " << (unsigned int)crazyflieCurrMemAddress << std::endl;
            }
            currMemAddress = crazyflieCurrMemAddress + dataRecivedSize;
            std::cout << "Current Memory Address: " << (unsigned int)currMemAddress << std::endl;
            break;

        case 2:
            ackRequestMemAddress = msgRecieved._sequence;

            if (ackRequestMemAddress != currMemAddress)
            {
                lostMemAddresses.push(ackRequestMemAddress);
                currMemAddress = ackRequestMemAddress;
            }
            if (lostMemAddresses.empty())
            {
                response._msgType = 0;
                response._sequence = currMemAddress;
            }
            else
            {
                response._msgType = 2;
                response._sequence = lostMemAddresses.front();
                lostMemAddresses.pop();
            }
            responseSize = sizeof(response._header);

            break;

        default:
            std::cout << "Incorrect Msg type" << currMemAddress << std::endl;
            response._msgType = 1;
            responseSize = sizeof(uint8_t);

            break;
        }
        crazyflie.sendAppChannelData(&response, responseSize);

    } while (currMemAddress < dataSize);
    outputFile.close();

    std::chrono::duration<double> delta = std::chrono::steady_clock::now() - start;
    std::cout << "Time [sec]: " << delta.count() << std::endl;
    std::cout << "rate[bytes/sec] : " << dataSize / (delta.count()) << std::endl;
    return 0;
}