#pragma once

#include <stdint.h>
#include <vector>
#include <string>

#include "USBDevice.h"

class CrazyflieUSB
  : public USBDevice
{
public:
  CrazyflieUSB(libusb_device *dev);

  ~CrazyflieUSB();

  void send(
    const uint8_t* data,
    uint32_t length);

  size_t recv(uint8_t *buffer, size_t max_length, unsigned int timeout);

private:
  void setCrtpToUsb(bool crtpToUsb);

};
