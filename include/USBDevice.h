#pragma once

#include <stdint.h>

#include <libusb.h>

namespace bitcraze {
namespace crazyflieLinkCpp {

class USBDevice
{
protected:
  USBDevice(
      libusb_device* dev);

  virtual ~USBDevice();

  void sendVendorSetup(
        uint8_t request,
        uint16_t value,
        uint16_t index,
        const unsigned char* data,
        uint16_t length);

protected:
  libusb_device* dev_;
  libusb_device_handle* dev_handle_;

  int versionMajor_;
  int versionMinor_;
};

} // namespace crazyflieLinkCpp
} // namespace bitcraze