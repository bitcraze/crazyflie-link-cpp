#include "LoggingCrazyflieWrapper.h"
#include <list>
#include <chrono>

using namespace bitcraze::crazyflieLinkCpp;

LoggingCrazyflieWrapper::LoggingCrazyflieWrapper(Crazyflie &crazyflie, const std::string &outputFilePath)
{
    _crazyflie = &crazyflie;
    _outputFilePath = outputFilePath;
}

LoggingCrazyflieWrapper::~LoggingCrazyflieWrapper()
{
}

void LoggingCrazyflieWrapper::start()
{
    _crazyflie->setParamByName("usd", "logging", 0, 1);
    _crazyflie->setParamByName("usd", "sendAppChannle", 1, 1);

    LoggingMsg msgRecieved;
    uint32_t dataSize = 0;
    uint32_t sizeOfBuffer = 0;
    uint32_t currPos = 0;
    uint32_t previousAckRequestLoc = 0;
    
    std::ofstream outputFile;
    if (!_outputFilePath.empty())
        outputFile.open(_outputFilePath, std::ifstream::binary | std::ios::out | std::ios::app);
    auto start = std::chrono::steady_clock::now();
    bool isFinished = false;
    while (!isFinished)
    {
        int8_t dataRecivedSize = this->recv(msgRecieved);
        uint8_t currChunkIndex = 0;
        if (dataRecivedSize < 0)
        {
            std::cout << "Error Receiving from crazyflie" << std::endl;
            break;
        }
        std::cout << (int)msgRecieved._msgType << "-> ";
        unsigned int ackRequestMemAddress = 0;
        uint32_t crazyflieCurrMemAddress = 0;
        // std::cout << "bytes received: " << (int)bytesRecieved << std::endl;
        if (_crazyflie->isRunning() == false)
        {
            this->sendResponse(ResponseMsgType::CLIENT_NOT_RUNNING);
            return;
        }
        switch (msgRecieved._msgType)
        {
        case (int)CrazyflieMsgType::DATA_SIZE_MSG:
            dataSize = msgRecieved._sequence;
            sizeOfBuffer = dataSize / 25;

            _buffer = (PacketDataPtr *)new PacketDataPtr[sizeOfBuffer];
            std::cout << "Data size: " << dataSize << std::endl;
            this->sendResponse(ResponseMsgType::SUCCESS);
            break;

        case (int)CrazyflieMsgType::DATA_MSG:
            crazyflieCurrMemAddress = msgRecieved._sequence;
            currChunkIndex = (crazyflieCurrMemAddress) / 25;

            if (nullptr != _buffer[currChunkIndex])
            {
                break;
            }

            _buffer[currChunkIndex] = (PacketDataPtr) new PacketData();
            std::copy_n(msgRecieved._data, sizeof(msgRecieved._data), *_buffer[currChunkIndex]);

            if (!_outputFilePath.empty())
            {
                outputFile.seekp(crazyflieCurrMemAddress);
                outputFile.write((const char *)msgRecieved._data, dataRecivedSize);
            }

            std::cout << "Current Memory Address: " << (unsigned int)crazyflieCurrMemAddress << std::endl;
            break;

        case (int)CrazyflieMsgType::ACK_REQUEST:
            ackRequestMemAddress = msgRecieved._sequence;

            currPos = std::find(_buffer+previousAckRequestLoc, _buffer + 80, nullptr) - _buffer;
            
            if (currPos != previousAckRequestLoc+80)
            {
                this->sendResponse(ResponseMsgType::NOT_ALL_DATA_RECEIVED, currPos);
            }
            else if (currPos + dataRecivedSize != ackRequestMemAddress)
            {
                std::cout << "Something Went Wrong in the Ack " << msgRecieved._msgType << std::endl;
                return;
            }
            else if( currPos + dataRecivedSize == dataSize)
            {
                std::cout << "Finished!" << msgRecieved._msgType << std::endl;
                isFinished = true;
                break;
            }
            else
            {
                this->sendResponse(ResponseMsgType::SUCCESS, crazyflieCurrMemAddress + dataRecivedSize);
                previousAckRequestLoc = crazyflieCurrMemAddress + dataRecivedSize;
            }
            break;

        default:
            std::cout << "Incorrect Msg type: " << msgRecieved._msgType << std::endl;
            this->sendResponse(ResponseMsgType::WRONG_TYPE);
            break;
        }
    }
    if (!_outputFilePath.empty())
        outputFile.close();
    std::chrono::duration<double> delta = std::chrono::steady_clock::now() - start;
    std::cout << "Time [sec]: " << delta.count() << std::endl;
    std::cout << "rate[bytes/sec] : " << dataSize / (delta.count()) << std::endl;
}

void LoggingCrazyflieWrapper::sendResponse(const ResponseMsgType &responseType, uint32_t data) const
{
    uint8_t response[5];
    response[0] = (int)responseType;
    std::copy_n((uint8_t *)&data, sizeof(uint32_t), response + 1);
    _crazyflie->sendAppChannelData(&response, sizeof(response));
}

void LoggingCrazyflieWrapper::sendResponse(const ResponseMsgType &responseType) const
{
    uint8_t response = (int)responseType;
    _crazyflie->sendAppChannelData(&response, sizeof(response));
}

int8_t LoggingCrazyflieWrapper::recv(LoggingMsg &loggingMsg) const
{
    return _crazyflie->recvAppChannelData(&loggingMsg, sizeof(loggingMsg)) - sizeof(loggingMsg._header);
}
