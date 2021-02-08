#pragma once

#include <vector>
#include <mutex>

#include <libusb-1.0/libusb.h>

#include "CrazyradioThread.h"
#include "CrazyflieUSBThread.h"

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
        return radioThreads_.size();
    }

    // std::vector<CrazyradioThread>& crazyradioThreads() {
    //     return radioThreads_;
    // }

    // const std::vector<libusb_device *>& crazyfliesOverUSB() const {
    //     return crazyfliesUSB_;
    // }

    void addConnection(std::shared_ptr<ConnectionImpl> con);

    void removeConnection(std::shared_ptr<ConnectionImpl> con);

private :
    USBManager();

private :
    libusb_context * ctx_;

    std::vector<CrazyflieUSBThread> crazyfliesUSB_;
    std::vector<CrazyradioThread> radioThreads_;

    std::mutex mutex_;
};
