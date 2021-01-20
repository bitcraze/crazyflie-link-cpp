#pragma once

#include <vector>
// #include <thread>

#include <libusb-1.0/libusb.h>

class USBManager {
public :
    // non construction-copyable
    USBManager(const USBManager &) = delete;

    // non copyable
    USBManager & operator=(const USBManager &) = delete;

    // deconstruct/cleanup
    ~USBManager();

    // void hotplugCrazyradioCallback(libusb_device *dev, libusb_hotplug_event event);
    // void hotplugCrazyflieUSBCallback(libusb_device *dev, libusb_hotplug_event event);
    static USBManager &get()
    {
        static USBManager instance;
        return instance;
    }

    size_t numCrazyfliesOverUSB() const {
        return crazyfliesUSB_.size();
    }

    size_t numCrazyradios() const {
        return crazyradios_.size();
    }

    const std::vector<libusb_device *>& crazyradios()
    {
        return crazyradios_;
    }

private :
    // constructor
    USBManager();

private :
    libusb_context * ctx_;
    std::vector<libusb_device*> crazyradios_;
    std::vector<libusb_device*> crazyfliesUSB_;

    // std::vector<std::thread>
        // radioThreads_;
};
