#include "crazyflieCppLink/Version.h"

const char * version_string =
#include "../version"
;

const char * version()
{
    // return
    // #include "version"
    // ;
    // // return "bla";
    return version_string;
}