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
    crazyflie.init();
    std::ofstream tocParamsFile;
    tocParamsFile.open("toc.csv");


    auto start = std::chrono::steady_clock::now();
    auto tocAndValues = crazyflie.getTocAndValues();

    for (auto element : tocAndValues)
    {
        TocItem& tocItem = element.first;
        auto val = element.second;

        tocParamsFile << (unsigned int)tocItem._paramId << "," << to_string(tocItem._paramAccessType) << "," << to_string(tocItem._paramType) << "," << tocItem._groupName << "," << tocItem._paramName << ",";
        if(tocItem.isFloat())
            tocParamsFile << val._floatVal << endl;
        else 
            tocParamsFile << (unsigned int)val._uintVal << endl;
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;

    cout << "------------------\n" << "numOfElements: " << tocAndValues.size() << "\n------------------\n\n\n" << endl;

    cout << "Get by name:                activeMarker . mode  \n";
    
    cout << crazyflie.getUIntByName("activeMarker", "mode") << endl;


    cout << "Get by name:                usd . logging  \n";
    cout << crazyflie.getUIntByName("usd", "logging") << endl;

    cout << "Time: " << (double)diff.count() << endl;

    cout << "file saved to build/ under the name toc.csv" << endl;
    tocParamsFile.close();

    return 0;
}
