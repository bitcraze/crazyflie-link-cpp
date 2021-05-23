#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>
#include <vector>

#include "Toc.h"

using namespace bitcraze::crazyflieLinkCpp;

int main()
{
    Connection con("usb://0");
    Toc toc(con);
    toc.run();

    return 0;
}