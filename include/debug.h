#pragma once
#include <iostream>


#define DEBUG_PRINT(var) (std::cout << #var << ": " << var << std::endl)
#define DEBUG_PRINT_WITH_CAST(var, cast) (std::cout << #var << ": " << (cast)var << std::endl)
#define DEBUG_PAUSE()  \
    {                  \
        int i = 0;     \
        std::cin >> i; \
    }

class Debug
{
    public:
    static bool passFlag;
    static int countSuccess;
    static int countError;
};
