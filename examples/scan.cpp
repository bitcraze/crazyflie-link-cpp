#include <iostream>
#include <thread>

#include "crazyflieCppLink/Connection.h"

int main()
{
    // scanning
    const auto uris = Connection::scan();

    for (const auto& uri : uris) {
        std::cout << uri << std::endl;
    }

    return 0;
}