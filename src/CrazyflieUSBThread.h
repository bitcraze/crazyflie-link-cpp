#pragma once

#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <libusb.h>

namespace bitcraze {
namespace crazyflieLinkCpp {

// forward declaration
class USBManager;
class ConnectionImpl;

class CrazyflieUSBThread
{
    friend class USBManager;
public:
    CrazyflieUSBThread(libusb_device * dev);
    CrazyflieUSBThread(CrazyflieUSBThread &&other);
    ~CrazyflieUSBThread();

    libusb_device* device() {
        return dev_;
    }

    bool hasError() const {
        return !runtime_error_.empty();
    }

private:
    void runWithErrorHandler();
    void run();

    void addConnection(std::shared_ptr<ConnectionImpl> con);

    void removeConnection(std::shared_ptr<ConnectionImpl> con);

private:
    libusb_device* dev_;

    std::mutex thread_mutex_;
    std::thread thread_;
    bool thread_ending_;

    std::shared_ptr<ConnectionImpl> connection_;
    std::string runtime_error_;
};

} // namespace crazyflieLinkCpp
} // namespace bitcraze