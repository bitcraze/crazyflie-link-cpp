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
        std::string strType = toc.getAccessAndStrType(tocItem._paramType).second;
        std::string strAccessType = toc.accessTypeToStr(toc.getAccessAndStrType(tocItem._paramType).first);
        cout << tocItem._paramId << ": " << strAccessType << ":" << strType << "  " << tocItem._groupName << "." << tocItem._paramName << " val=";
        if(strType.find("int") != std::string::npos)
            cout << param.getUIntById(tocItem._paramId) << endl;
        else 
            cout << param.getFloatById(tocItem._paramId) << endl;
    }
    cout << "------------------\n" << "numOfElements: " << (int) numOfElements << "\n------------------\n\n\n" << endl;

    cout << "Get by name:                activeMarker . mode  \n";
    
    cout << param.getUIntByName("activeMarker", "mode") << endl;


    cout << "Get by name:                usd . logging  \n";
    cout << param.getUIntByName("usd", "logging") << endl;

    return 0;
}