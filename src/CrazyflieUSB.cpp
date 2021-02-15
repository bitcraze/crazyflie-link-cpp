#include "CrazyflieUSB.h"

#include <sstream>
#include <stdexcept>

#include <libusb.h>

namespace bitcraze {
namespace crazyflieLinkCpp {

CrazyflieUSB::CrazyflieUSB(libusb_device *dev)
    : USBDevice(dev)
{
    setCrtpToUsb(true);
}

CrazyflieUSB::~CrazyflieUSB()
{
    try {
        setCrtpToUsb(false);
    } catch(...) {
        // do not throw in dtor
    }
}

bool CrazyflieUSB::send(
    const uint8_t* data,
    uint32_t length)
{
    int transferred = 0;

    // Send data
    int status = libusb_bulk_transfer(
        dev_handle_,
        /* endpoint*/ (0x01 | LIBUSB_ENDPOINT_OUT),
        (uint8_t*)data,
        length,
        &transferred,
        /*timeout*/ 10);
    if (status == LIBUSB_ERROR_TIMEOUT) {
        if (transferred == (int)length) {
            // If actually everything was transferred, avoid sending the same packet again.
            return true;
        } else if (transferred > 0) {
            // Fixing partial transfers would require a protocol change, so throw an exception instead.
            std::stringstream sstr;
            sstr << "Timeout, but already transferred " << transferred << " of " << length << " bytes!";
            throw std::runtime_error(sstr.str());
        }
        return false;
    }
    if (status != LIBUSB_SUCCESS) {
        throw std::runtime_error(libusb_error_name(status));
    }
    if (length != (uint32_t)transferred) {
        std::stringstream sstr;
        sstr << "Did transfer " << transferred << " but " << length << " was requested!";
        throw std::runtime_error(sstr.str());
    }
    return true;
}

size_t CrazyflieUSB::recv(uint8_t *buffer, size_t max_length, unsigned int timeout)
{
    int transferred;

    // Read result
    int status = libusb_bulk_transfer(
        dev_handle_,
        /* endpoint*/ (0x81 | LIBUSB_ENDPOINT_IN),
        buffer,
        max_length,
        &transferred,
        /*timeout*/ timeout);
    if (status != LIBUSB_SUCCESS && status != LIBUSB_ERROR_TIMEOUT) {
        throw std::runtime_error(libusb_error_name(status));
    }
    return transferred;
}

void CrazyflieUSB::setCrtpToUsb(bool crtpToUsb)
{
    sendVendorSetup(0x01, 0x01, crtpToUsb, NULL, 0);
}

} // namespace crazyflieLinkCpp
} // namespace bitcraze