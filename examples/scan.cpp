#include <iostream>
#include <thread>

#include "crazyflieCppLink/Connection.h"

using namespace bitcraze::crazyflieCppLink;

int main()
{
    // scanning
    const auto uris = Connection::scan();

    for (const auto uri : uris) {
        std::cout << uri << std::endl;
    }

    return 0;
}