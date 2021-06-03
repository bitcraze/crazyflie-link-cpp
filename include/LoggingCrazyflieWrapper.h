#pragma once

#include "Crazyflie.h"


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

    uint8_t _data[25];
};
#pragma pack(pop)

class LoggingCrazyflieWrapper
{
private:
    Crazyflie* _crazyflie;
    std::string _outputFilePath;

public:
    void start();
    LoggingCrazyflieWrapper(Crazyflie& crazyflie, const std::string& outputFilePath = "");
    ~LoggingCrazyflieWrapper();
};

