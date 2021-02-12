#include "crazyflieLinkCpp/Version.h"

namespace bitcraze {
namespace crazyflieLinkCpp {

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

} // namespace crazyflieLinkCpp
} // namespace bitcraze