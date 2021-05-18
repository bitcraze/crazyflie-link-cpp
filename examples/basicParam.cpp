#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>
#include <vector>
#include <byteswap.h>

#include "crazyflieLinkCpp/Param.h"
#include "crazyflieLinkCpp/Toc.h"

using namespace bitcraze::crazyflieLinkCpp;

int main()
{
    Connection con("usb://0");
    Toc toc(con);
    Param param(con);
    uint16_t numOfElements = toc.getTocInfo()._numberOfElements;

    for (uint16_t i = 0; i < numOfElements; i++)
    {
        auto tocItem = toc.getItemFromToc(i);
        std::string strType = toc.getAccessAndStrType(tocItem._paramType).second;
        std::string strAccessType = toc.accessTypeToStr(toc.getAccessAndStrType(tocItem._paramType).first);
        std::cout << tocItem._paramId << ": " << strAccessType << ":" << strType << "  " << tocItem._groupName << "." << tocItem._paramName << " val=";

        if (strType.find("int") != std::string::npos)
        {
            std::cout << (int)param.getUInt(i);
        }
        else if ("float" == strType)
        {
            std::cout << param.getFloat(i);
        }
        std::cout << std::endl;
    }
    std::cout << "numOfElements: " << (int) numOfElements << std::endl;

    return 0;
}