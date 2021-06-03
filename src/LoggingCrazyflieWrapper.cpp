#include "LoggingCrazyflieWrapper.h"

using namespace bitcraze::crazyflieLinkCpp;

LoggingCrazyflieWrapper::LoggingCrazyflieWrapper(Crazyflie& crazyflie, const std::string& outputFilePath) 
{
    _crazyflie = &crazyflie;
    _ouputFilePath = outputFilePath;
}

LoggingCrazyflieWrapper::~LoggingCrazyflieWrapper()
{
}



void LoggingCrazyflieWrapper::start() 
{
    crazyflie.setParamByName("usd", "logging", 0, 1);
    crazyflie.setParamByName("usd", "sendAppChannle", 1, 1);
}
