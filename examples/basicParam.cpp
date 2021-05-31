#include <iostream>
#include <chrono>
#include <fstream>
#include "Crazyflie.h"


using std::cout;
using std::endl;
using namespace bitcraze::crazyflieLinkCpp;

int main()
{
    Crazyflie crazyflie("usb://0");

    std::ofstream tocParamsFile;
    tocParamsFile.open("toc.csv");


    uint16_t numOfElements = crazyflie.getTocInfo()._numberOfElements;
    auto start = std::chrono::steady_clock::now();

    for (uint16_t i = 0; i < numOfElements; i++)
    {
        auto tocItem = crazyflie.getItemFromToc(i);
        std::string strType = crazyflie.getAccessAndStrType(tocItem._paramType).second;
        std::string strAccessType = crazyflie.accessTypeToStr(crazyflie.getAccessAndStrType(tocItem._paramType).first);
        cout << tocItem._paramId << ": " << strAccessType << ":" << strType << "  " << tocItem._groupName << "." << tocItem._paramName << " val=";
        if(strType.find("int") != std::string::npos)
            cout << crazyflie.getUIntById(tocItem._paramId) << endl;
        else 
            cout << crazyflie.getFloatById(tocItem._paramId) << endl;
        //================================write to file==================================
        tocParamsFile << tocItem._paramId << "," << strAccessType << "," << strType << "," << tocItem._groupName << "," << tocItem._paramName << ",";
        if(strType.find("int") != std::string::npos)
            tocParamsFile << crazyflie.getUIntById(tocItem._paramId) << endl;
        else 
            tocParamsFile << crazyflie.getFloatById(tocItem._paramId) << endl;
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;

    cout << "------------------\n" << "numOfElements: " << (int) numOfElements << "\n------------------\n\n\n" << endl;

    cout << "Get by name:                activeMarker . mode  \n";
    
    cout << crazyflie.getUIntByName("activeMarker", "mode") << endl;


    cout << "Get by name:                usd . logging  \n";
    cout << crazyflie.getUIntByName("usd", "logging") << endl;

    cout << "Time: " << (double)diff.count() << endl;

    cout << "file saved to build/ under the name toc.csv" << endl;

    // crazyflie.printToc();

    tocParamsFile.close();

    return 0;
}