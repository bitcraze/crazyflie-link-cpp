#pragma once

#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>

#include <libusb.h>

namespace bitcraze {
namespace crazyflieLinkCpp {

// forward declaration
class USBManager;
class ConnectionImpl;

class CrazyradioThread
{
    friend class USBManager;
public:
    CrazyradioThread(libusb_device * dev);
    CrazyradioThread(CrazyradioThread &&other);
    ~CrazyradioThread();

    // bool isActive() const;

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

    std::mutex connections_mutex_;
    std::condition_variable connections_updated_cv_;
    bool connections_updated_;
    std::set<std::shared_ptr<ConnectionImpl>> connections_;
    std::string runtime_error_;
};

} // namespace crazyflieLinkCpp
} // namespace bitcraze