#include "crazyflieCppLink/Version.h"

namespace bitcraze {
namespace crazyflieCppLink {

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

} // namespace crazyflieCppLink
} // namespace bitcraze