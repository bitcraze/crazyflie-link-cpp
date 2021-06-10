#include "LoggingCrazyflieWrapper.h"
#include <list>
#include <chrono>
#include <thread>

#define ACK_DELAY_MICRO_SEC 30

using namespace bitcraze::crazyflieLinkCpp;

LoggingCrazyflieWrapper::LoggingCrazyflieWrapper(Crazyflie &crazyflie, const std::string &outputFilePath)
{
    _crazyflie = &crazyflie;
    _outputFilePath = outputFilePath;
}

LoggingCrazyflieWrapper::~LoggingCrazyflieWrapper()
{
}

void LoggingCrazyflieWrapper::start(bool withDebugging)
{

    _crazyflie->setParamByName("usd", "logging", 0, 1);
    _crazyflie->setParamByName("usd", "sendAppChannle", 1, 1);

    std::vector<uint8_t> result;
    uint32_t currMemAddress = 0;
    uint32_t dataSize = 0;
    std::ofstream outputFile(_outputFilePath);

    auto start = std::chrono::steady_clock::now();

    do
    {
        result = _crazyflie->recvAppChannelData();
        if (result.empty())
        {
            if (withDebugging)
                std::cout << "Error Receiving from crazyflie" << std::endl;
            break;
        }
        uint8_t packetCode = result[0];
        if (withDebugging)
            std::cout << (int)packetCode << "-> ";
        uint8_t response[5] = {0};
        unsigned int ackRequestMemAddress = 0;
        uint32_t crazyflieCurrMemAddress = 0;


        switch (packetCode)
        {
        case 0:
            std::copy_n(result.begin() + 1, sizeof(dataSize), (uint8_t *)&dataSize);
            if (withDebugging)
                std::cout << "Data size: " << dataSize << std::endl;
            response[0] = 0;
            _crazyflie->sendAppChannelData(&response[0], sizeof(uint8_t)); //size msg ack
            break;

        case 1:
            std::copy_n(result.begin() + 1, sizeof(crazyflieCurrMemAddress), (uint8_t *)&crazyflieCurrMemAddress);
            //send not all data recieved
            if (crazyflieCurrMemAddress != currMemAddress)
            {
                break;
            }
            currMemAddress += result.size() - sizeof(currMemAddress) - sizeof(uint8_t);
            if (withDebugging)
                std::cout << "Current Memory Address: " << (unsigned int)currMemAddress << std::endl;
            outputFile.write(reinterpret_cast<char *>(&result[5]), result.size() - 5);

            break;

        case 2:
            std::copy_n(result.begin() + 1, sizeof(ackRequestMemAddress), (uint8_t *)&ackRequestMemAddress);
            std::this_thread::sleep_for(std::chrono::microseconds(ACK_DELAY_MICRO_SEC));

            if (ackRequestMemAddress == currMemAddress)
            {
                if (withDebugging)
                    std::cout << "Ack Request Mem Address: " << ackRequestMemAddress << std::endl;

                response[0] = 0;
            }
            else
            {
                if (withDebugging)
                    std::cout << "Wrong Memory Address: " << (unsigned int)currMemAddress << std::endl;

                response[0] = 2;
            }

            std::copy_n((uint8_t *)&currMemAddress, sizeof(currMemAddress), response + 1);
            _crazyflie->sendAppChannelData(response, sizeof(response)); //data msg ack
            break;

        default:
            if (withDebugging)
                std::cout << "Incorrect Msg type" << currMemAddress << std::endl;
            response[0] = 1;
            _crazyflie->sendAppChannelData(response, sizeof(uint8_t));

            break;
        }

    } while (currMemAddress < dataSize);
    outputFile.close();

    std::chrono::duration<double> delta = std::chrono::steady_clock::now() - start;
    if (withDebugging)
    {
        std::cout << "Time [sec]: " << delta.count() << std::endl;
        std::cout << "rate[bytes/sec] : " << dataSize / (delta.count()) << std::endl;
    }
    else
    {
        std::cout << dataSize / (delta.count()) << std::endl;
    }
}

