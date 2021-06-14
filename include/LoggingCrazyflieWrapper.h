#pragma once

#include "Crazyflie.h"
#include "debug.h"

enum CrazyflieMsgType
{
    DATA_SIZE_MSG,
    DATA_MSG,
    ACK_REQUEST
};

enum ResponseMsgType
{
    RESPONSE_SUCCESS,
    WRONG_TYPE,
    NOT_ALL_DATA_RECEIVED,
    CLIENT_NOT_RUNNING
};

class LoggingCrazyflieWrapper
{
private:
    Crazyflie *_crazyflie;
    std::string _outputFilePath;

public:
    void start(bool withDebugging = false);
    LoggingCrazyflieWrapper(Crazyflie &crazyflie, const std::string &outputFilePath = "");
    ~LoggingCrazyflieWrapper();
};
