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

        cout << param.getById(tocItem._paramId) << endl;
    }
    cout << "numOfElements: " << (int) numOfElements << endl;
    cout << "------------------" << endl;

    cout << "Get by name:\n";
    cout << param.getByName("activeMarker", "mode") << endl;

    return 0;
}