#include <iostream>
#include <thread>

#include "crazyflieLinkCpp/Connection.h"

using namespace bitcraze::crazyflieLinkCpp;

int main()
{
    // scanning
    const auto uris = Connection::scan();

    for (const auto& uri : uris) {
        std::cout << uri << std::endl;
    }

    return 0;
}