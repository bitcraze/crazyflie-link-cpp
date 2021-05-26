#include <iostream>

#include "Crazyflie.h"

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
    Crazyflie crazyflie("usb://0");
    
    std::string requestedGroupName = argv[1];
    std::string requestedParamName = argv[2];
    size_t newValueSize = std::stoul(argv[4]);

    uint16_t numOfElements = crazyflie.getTocInfo()._numberOfElements;

    std::string strType ="";
    for (uint16_t i = 0; i < numOfElements; i++)
    {
        auto tocItem = crazyflie.getItemFromToc(i);
        // std::string strType = crazyflie.getAccessAndStrType(tocItem._paramType).second;
        // std::string strAccessType = crazyflie.accessTypeToStr(crazyflie.getAccessAndStrType(tocItem._paramType).first);
        strType = crazyflie.getAccessAndStrType(tocItem._paramType).second;
        if(tocItem._groupName == requestedGroupName && tocItem._paramName == requestedParamName)
        {
            
            if("float" == strType)
            {
                cout << crazyflie.getFloatByName(requestedGroupName, requestedParamName) << endl;
                cout << "input: " << std::stof(argv[3])<< endl;
                crazyflie.setParam(tocItem._paramId, std::stof(argv[3]));
            }
            else
            {
                cout << crazyflie.getUIntByName(requestedGroupName, requestedParamName) << endl;
                crazyflie.setParam(tocItem._paramId, std::stoul(argv[3]), newValueSize);
            }
           
            break;
        }
    }
    cout << "Get by name:                \n";


        if("float" == strType)
        {
            cout << crazyflie.getFloatByName(requestedGroupName, requestedParamName) << endl;
        }
        else
        {
            cout << crazyflie.getUIntByName(requestedGroupName, requestedParamName) << endl;
        }

    return 0;
}