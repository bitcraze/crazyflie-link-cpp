#include <iostream>

#include "Crazyflie.h"

using std::cout;
using std::endl;
using namespace bitcraze::crazyflieLinkCpp;

int main(int argc, char *argv[])
{
    if(argc < 5 && argv)
    {
        std::cout << "<group name> <param name> <new value> <byte size>" << std::endl;
        return 0;
    }
    Crazyflie crazyflie("usb://0");
    crazyflie.init();
    
    std::string group = argv[1];
    std::string name = argv[2];
    size_t newValueSize = std::stoul(argv[4]);
    if(crazyflie.isParamFloat(group, name))
    {
        std::cout << "Initial value: " << crazyflie.getFloatByName(group, name) << std::endl;
        crazyflie.setParamByName(group,name,std::stof(argv[3]));
        std::cout << "New value: " << crazyflie.getFloatByName(group, name) << std::endl;
    }
    else
    {
        std::cout << "Initial value: " << crazyflie.getUIntByName(group, name) << std::endl;
        crazyflie.setParamByName(group,name,std::stof(argv[3]), newValueSize);
        std::cout << "New value: " << crazyflie.getUIntByName(group, name) << std::endl;
    }
    return 0;
}