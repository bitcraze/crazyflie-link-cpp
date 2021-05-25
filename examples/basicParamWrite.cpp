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
    size_t newValueSize = std::stoul(argv[4]);
    uint16_t numOfElements = toc.getTocInfo()._numberOfElements;
    std::string strType ="";
    for (uint16_t i = 0; i < numOfElements; i++)
    {
        auto tocItem = toc.getItemFromToc(i);
        // std::cout << i << std::endl;
        // std::string strType = toc.getAccessAndStrType(tocItem._paramType).second;
        // std::string strAccessType = toc.accessTypeToStr(toc.getAccessAndStrType(tocItem._paramType).first);
        strType = toc.getAccessAndStrType(tocItem._paramType).second;
        if(tocItem._groupName == requestedGroupName && tocItem._paramName == requestedParamName)
        {
            
            if("float" == strType)
            {
                cout << param.getFloatByName(requestedGroupName, requestedParamName) << endl;
                cout << "input: " << std::stof(argv[3])<< endl;
                param.setParam(tocItem._paramId, std::stof(argv[3]));
            }
            else
            {
                cout << param.getUIntByName(requestedGroupName, requestedParamName) << endl;
                param.setParam(tocItem._paramId, std::stoul(argv[3]), newValueSize);
            }
           
            break;
        }
    }
    cout << "Get by name:                \n";


        if("float" == strType)
        {
            cout << param.getFloatByName(requestedGroupName, requestedParamName) << endl;
        }
        else
        {
            cout << param.getUIntByName(requestedGroupName, requestedParamName) << endl;
        }

    return 0;
}