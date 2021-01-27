#pragma once

#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <libusb-1.0/libusb.h>

// forward declaration
class USBManager;
class ConnectionImpl;

class CrazyradioThread
{
    friend class USBManager;
public:
    CrazyradioThread(libusb_device * dev);

    CrazyradioThread(CrazyradioThread &&other)
    {
        std::unique_lock<std::mutex> rhs_lk1(other.thread_mutex_, std::defer_lock);
        std::unique_lock<std::mutex> rhs_lk2(other.connections_mutex_, std::defer_lock);
        std::lock(rhs_lk1, rhs_lk2);
        dev_ = std::move(other.dev_);
        thread_ = std::move(other.thread_);
        thread_ending_ = std::move(other.thread_ending_);
        connections_ = std::move(other.connections_);
    }

    ~CrazyradioThread();

    // bool isActive() const;

    libusb_device* device() {
        return dev_;
    }

private: 
    void run();

    void addConnection(std::shared_ptr<ConnectionImpl> con);

    void removeConnection(std::shared_ptr<ConnectionImpl> con);

private:
    libusb_device* dev_;

    std::mutex thread_mutex_;
    std::thread thread_;
    bool thread_ending_;

    std::mutex connections_mutex_;
    // std::condition_variable connections_updated_cv_;
    // bool connections_updated_;
    std::set<std::shared_ptr<ConnectionImpl>> connections_;
};