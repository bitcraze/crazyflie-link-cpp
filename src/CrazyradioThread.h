#pragma once

#include <set>
#include <thread>

#include <libusb-1.0/libusb.h>

// forward declaration
class Connection;

class CrazyradioThread
{
public:
    CrazyradioThread(libusb_device * dev);

    bool isActive() const;

    libusb_device* device() {
        return dev_;
    }

    void addConnection(Connection* con);

    void removeConnection(Connection* con);

private: 
    void run();

private:
    libusb_device* dev_;
    std::thread thread_;

    // TODO: this needs to be threadsafe!
    std::set<Connection*> connections_;
};