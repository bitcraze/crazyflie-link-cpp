#pragma once

#include "Crazyflie.h"
#include <deque>

enum class CrazyflieMsgType
{
    DATA_SIZE_MSG,
    DATA_MSG,
    ACK_REQUEST
};

enum class ResponseMsgType
{
    SUCCESS,
    WRONG_TYPE,
    NOT_ALL_DATA_RECEIVED,
    CLIENT_NOT_RUNNING
};

typedef uint8_t PacketData[25];
typedef PacketData *PacketDataPtr;

#pragma pack(push, 1)
struct LoggingMsg
{
    union
    {
        struct
        {
            uint8_t _msgType;
            uint32_t _sequence;
        };
        uint8_t _header[5];
    };

    PacketData _data;
};
#pragma pack(pop)

class LoggingCrazyflieWrapper
{
private:
    Crazyflie *_crazyflie;
    uint8_t** _buffer = {nullptr};
    std::string _outputFilePath;

public:
    void start(bool withDebugging = false);
    LoggingCrazyflieWrapper(Crazyflie &crazyflie, const std::string &outputFilePath = "");
    ~LoggingCrazyflieWrapper();
    void sendResponse(const ResponseMsgType &responseType, uint32_t data) const;
    void sendResponse(const ResponseMsgType &responseType) const;

    int8_t recv(LoggingMsg &loggingMsg) const; //returns amount of data bytes recieved. if return < 0 then an error occured
};
