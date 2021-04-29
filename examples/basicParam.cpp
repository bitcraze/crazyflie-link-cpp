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
    Param param("usb://0");
    Toc toc("usb://0");
    uint16_t numOfElements = toc.getTocInfo()._numberOfElements;

    for(uint16_t i = 0; i < numOfElements; i++)
    {
        toc.getStrType(toc.getItemFromToc(i)._paramType);
    }

    return 0;
}