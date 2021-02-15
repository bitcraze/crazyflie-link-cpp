#pragma once

#include <map>
#include <vector>
#include <mutex>

#include <libusb.h>

#include "CrazyradioThread.h"
#include "CrazyflieUSBThread.h"

namespace bitcraze {
namespace crazyflieLinkCpp {

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

    // std::vector<CrazyradioThread>& crazyradioThreads() {
    //     return radioThreads_;
    // }

    // const std::vector<libusb_device *>& crazyfliesOverUSB() const {
    //     return crazyfliesUSB_;
    // }

    void addConnection(std::shared_ptr<ConnectionImpl> con);

    void removeConnection(std::shared_ptr<ConnectionImpl> con);
    
    void updateDevices();

private :
    USBManager();

    std::string tryToQuerySerialNumber(libusb_device *dev, const libusb_device_descriptor *deviceDescriptor);

private :
    libusb_context * ctx_;

    // serial number -> devId
    std::map<std::string, size_t> crazyfliesUSB_devIdbySN_;
    // devId -> thread
    std::map<size_t, CrazyflieUSBThread> crazyfliesUSB_;
    size_t crazyfliesUSB_lastDevId_;
    
    // serial number -> devId
    std::map<std::string, size_t> crazyradios_devIdbySN_;
    // devId -> thread
    std::map<size_t, CrazyradioThread> crazyradios_;
    size_t crazyradios_lastDevId_;

    std::mutex mutex_;
};

} // namespace crazyflieLinkCpp
} // namespace bitcraze