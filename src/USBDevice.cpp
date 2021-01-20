#include "USBDevice.h"

#include <sstream>
#include <stdexcept>

USBDevice::USBDevice(
    libusb_device *dev)
    : dev_(dev)
{
    int err = libusb_open(dev, &dev_handle_);
    if (err != LIBUSB_SUCCESS)
    {
        throw std::runtime_error(libusb_error_name(err));
    }

    err = libusb_set_configuration(dev_handle_, 1);
    if (err != LIBUSB_SUCCESS)
    {
        throw std::runtime_error(libusb_error_name(err));
    }

    err = libusb_claim_interface(dev_handle_, 0);
    if (err != LIBUSB_SUCCESS)
    {
        throw std::runtime_error(libusb_error_name(err));
    }
}


USBDevice::~USBDevice()
{
    if (dev_handle_) {
        // ignore the error (we don't want to throw in dtor)
        libusb_release_interface(dev_handle_, 0);

        // function returns void => no error checking
        libusb_close(dev_handle_);
    }
}

void USBDevice::sendVendorSetup(
    uint8_t request,
    uint16_t value,
    uint16_t index,
    const unsigned char* data,
    uint16_t length)
{
    int err = libusb_control_transfer(
        dev_handle_,
        LIBUSB_REQUEST_TYPE_VENDOR,
        request,
        value,
        index,
        (unsigned char*)data,
        length,
        /*timeout*/ 1000);
    if (err != LIBUSB_SUCCESS) {
        throw std::runtime_error(libusb_error_name(err));
    }
}
