#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>
#include <vector>
#include <thread>
#include "LoggingCrazyflieWrapper.h"



int main()
{
    Crazyflie crazyflie("usb://0");
    LoggingCrazyflieWrapper logging(crazyflie);
    crazyflie.init();

    logging.start();


    return 0;
}