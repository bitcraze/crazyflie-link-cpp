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
    std::cout << "numOfElements: " << (int) numOfElements << std::endl;

    for (uint16_t i = 0; i < numOfElements; i++)
    {
        auto tocItem = toc.getItemFromToc(i);
        std::string strType = toc.getAccessAndStrType(tocItem._paramType).second;
        std::string strAccessType = toc.accessTypeToStr(toc.getAccessAndStrType(tocItem._paramType).first);
        std::cout << tocItem._paramId << ": " << strAccessType << ":" << strType << "  " << tocItem._groupName << "." << tocItem._paramName << " val=";

        if ("uint8_t" == strType)
        {
            std::cout << (int)param.getParam<uint8_t>(i);
        }
        else if ("uint16_t" == strType)
        {
            std::cout << (int)param.getParam<uint16_t>(i);
        }
        else if ("uint32_t" == strType)
        {
            std::cout << (unsigned int)param.getParam<uint32_t>(i);
        }
        else if ("uint64_t" == strType)
        {
            std::cout << param.getParam<uint64_t>(i);
        }
        else if ("int8_t" == strType)
        {
            std::cout << (int)param.getParam<int8_t>(i);
        }
        else if ("int16_t" == strType)
        {
            std::cout << (int)param.getParam<int16_t>(i);
        }
        else if ("int32_t" == strType)
        {
            std::cout << (int)param.getParam<int32_t>(i);
        }
        else if ("int64_t" == strType)
        {
            std::cout << param.getParam<int64_t>(i);
        }
        else if ("FP16" == strType)
        {
            // std::cout << (int)param.getParam<__fp16>(i);
        }
        else if ("float" == strType)
        {
            std::cout << param.getParam<float>(i);
        }
        else if ("double" == strType)
        {
            std::cout << param.getParam<double>(i);
        }
        std::cout << std::endl;
    }

    return 0;
}