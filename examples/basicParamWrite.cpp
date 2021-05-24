#include <iostream>

#include "Param.h"

using std::cout;
using std::endl;
using namespace bitcraze::crazyflieLinkCpp;

int main(int argc, char *argv[])
{
    if(argc < 5)
    {
        std::cout << "<group name> <param name> <new value> <byte size>" << std::endl;
        return 0;
    }
    Connection con("usb://0");
    Param param(con);
    Toc toc = param.getToc();
    
    std::string requestedGroupName = argv[1];
    std::string requestedParamName = argv[2];
    unsigned int newValue = std::stoul(argv[3]);
    size_t newValueSize = std::stoul(argv[4]);
    uint16_t numOfElements = toc.getTocInfo()._numberOfElements;

    for (uint16_t i = 0; i < numOfElements; i++)
    {
        auto tocItem = toc.getItemFromToc(i);
        // std::cout << i << std::endl;
        // std::string strType = toc.getAccessAndStrType(tocItem._paramType).second;
        // std::string strAccessType = toc.accessTypeToStr(toc.getAccessAndStrType(tocItem._paramType).first);

        if(tocItem._groupName == requestedGroupName && tocItem._paramName == requestedParamName)
        {
            cout << param.getUIntByName(requestedGroupName, requestedParamName) << endl;

            param.setParam(tocItem._paramId, newValue, newValueSize);
            break;
        }
    }
    cout << "Get by name:                \n";
    cout << param.getUIntByName(requestedGroupName, requestedParamName) << endl;

    return 0;
}