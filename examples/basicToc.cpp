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

    return 0;
}