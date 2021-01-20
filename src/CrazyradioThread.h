#pragma once

#include <libusb-1.0/libusb.h>

class CrazyradioThread
{
public:
    void operator()(libusb_context *ctx, libusb_device *dev);

// private:
    // libusb_context *ctx_;
    // libusb_device_handle *devHandle_;
};