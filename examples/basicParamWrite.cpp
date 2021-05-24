#include <iostream>

#include "Param.h"

using std::cout;
using std::endl;
using namespace bitcraze::crazyflieLinkCpp;

int main()
{
    Connection con("usb://0");
    Param param(con);
    Toc toc = param.getToc();

    uint16_t numOfElements = toc.getTocInfo()._numberOfElements;

    for (uint16_t i = 0; i < numOfElements; i++)
    {
        auto tocItem = toc.getItemFromToc(i);
        // std::cout << i << std::endl;
        // std::string strType = toc.getAccessAndStrType(tocItem._paramType).second;
        // std::string strAccessType = toc.accessTypeToStr(toc.getAccessAndStrType(tocItem._paramType).first);

        if(tocItem._groupName == "usd" && tocItem._paramName == "logging")
        {
            cout << param.getUIntByName("usd", "logging") << endl;

            param.setParam(tocItem._paramId, 1, sizeof(uint8_t));
            break;
        }
    }
    cout << "Get by name:                usd . logging  \n";
    cout << param.getUIntByName("usd", "logging") << endl;

    return 0;
}