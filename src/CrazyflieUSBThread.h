#pragma once

#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <libusb-1.0/libusb.h>

// forward declaration
class USBManager;
class ConnectionImpl;

class CrazyflieUSBThread
{
    friend class USBManager;
public:
    CrazyflieUSBThread(libusb_device * dev);

    CrazyflieUSBThread(CrazyflieUSBThread &&other)
    {
        const std::lock_guard<std::mutex> lk(other.thread_mutex_);
        dev_ = std::move(other.dev_);
        thread_ = std::move(other.thread_);
        thread_ending_ = std::move(other.thread_ending_);
        connection_ = std::move(other.connection_);
        // runtime_error_ = std::move(other.runtime_error_);
    }

    ~CrazyflieUSBThread();

    libusb_device* device() {
        return dev_;
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
    // std::string runtime_error_;
};