#include "CrazyradioThread.h"

void CrazyradioThread::operator()(libusb_context *ctx, libusb_device *dev)
{
    libusb_device_handle * dev_handle;
    int err = libusb_open(dev, &dev_handle);
    if (err != LIBUSB_SUCCESS) {
        return;
    }
}