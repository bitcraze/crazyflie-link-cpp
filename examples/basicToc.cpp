#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>
#include <vector>

#include "Crazyflie.h"

using namespace bitcraze::crazyflieLinkCpp;

int main()
{
    Crazyflie crazyflie("usb://0");
    crazyflie.init();
    crazyflie.printToc();
    std::cout << "number six: "<< crazyflie.getItemFromToc(6) << std::endl;
    std::cout << "number six val: "<< crazyflie.getUIntByName("imu_sensors", "BMP388") << std::endl;

    return 0;
}