#include "CrazyflieUSB.h"

#include <sstream>
#include <stdexcept>
#include <iostream>

#include <libusb-1.0/libusb.h>

CrazyflieUSB::CrazyflieUSB(libusb_device *dev)
    : USBDevice(dev)
{
    setCrtpToUsb(true);
}

CrazyflieUSB::~CrazyflieUSB()
{
    setCrtpToUsb(false);
}

void CrazyflieUSB::send(
    const uint8_t* data,
    uint32_t length)
{
    int transferred;

    // Send data
    int status = libusb_bulk_transfer(
        dev_handle_,
        /* endpoint*/ (0x01 | LIBUSB_ENDPOINT_OUT),
        (uint8_t*)data,
        length,
        &transferred,
        /*timeout*/ 1000);
    if (status != LIBUSB_SUCCESS) {
        throw std::runtime_error(libusb_error_name(status));
    }
    if (length != (uint32_t)transferred) {
        std::stringstream sstr;
        sstr << "Did transfer " << transferred << " but " << length << " was requested!";
        throw std::runtime_error(sstr.str());
    }
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
    std::cout << status << " " << transferred << std::endl;
    return transferred;
}

void CrazyflieUSB::setCrtpToUsb(bool crtpToUsb)
{
    std::cout << "setCrtpToUsb" << std::endl;
    sendVendorSetup(0x01, 0x01, crtpToUsb, NULL, 0);
}
