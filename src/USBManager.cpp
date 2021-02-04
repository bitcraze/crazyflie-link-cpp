#include "USBManager.h"

#include <stdexcept>
#include <functional>
#include <iostream>
#include <cassert>
#include <sstream>
#include <map>

// #include "native_link/Connection.h"
#include "ConnectionImpl.h"

// extern "C" int hotplugCrazyradioCallback_C(libusb_context */*ctx*/, libusb_device *dev,
//                                                   libusb_hotplug_event event, void *user_data)
// {
//     reinterpret_cast<USBManager*>(user_data)->hotplugCrazyradioCallback(dev, event);
//     return 0;
// }

// extern "C" int hotplugCrazyflieUSBCallback_C(libusb_context * /*ctx*/, libusb_device *dev,
//                                          libusb_hotplug_event event, void *user_data)
// {
//     reinterpret_cast<USBManager *>(user_data)->hotplugCrazyflieUSBCallback(dev, event);
//     return 0;
// }

class ConnectionImpl;

USBManager::USBManager()
{
    int result = libusb_init(&ctx_);
    if (result != LIBUSB_SUCCESS)
    {
        throw std::runtime_error(libusb_error_name(result));
    }

    // 1. Find all Crazyflies and Crazyradios connected over USB

    // b. discover devices
    libusb_device **list;
    ssize_t cnt = libusb_get_device_list(ctx_, &list);

    // c. for each device, check if it is a Crazyflie or Crazyradio
    for (ssize_t i = 0; i < cnt; i++)
    {
        libusb_device *device = list[i];
        libusb_device_descriptor deviceDescriptor;
        int err = libusb_get_device_descriptor(device, &deviceDescriptor);
        if (err == LIBUSB_SUCCESS)
        {
            // Crazyflie over USB
            if (deviceDescriptor.idVendor == 0x0483 &&
                deviceDescriptor.idProduct == 0x5740)
            {
                libusb_ref_device(device);
                crazyfliesUSB_.push_back(device);
            }
            // Crazyradio
            else if (deviceDescriptor.idVendor == 0x1915 &&
                     deviceDescriptor.idProduct == 0x7777)
            {
                libusb_ref_device(device);
                radioThreads_.emplace_back(CrazyradioThread(device));
            }
        }
    }
    // function returns void => no error checking
    libusb_free_device_list(list, 1);


    // if (libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG) == 0) {
    //     // TODO: Implement alternative scan that just enumerates devices!
    //     throw std::runtime_error("libusb does not support hotplug!");
    // }

    //     // register (&enumerate) all Crazyradios
    //     result = libusb_hotplug_register_callback(ctx_,
    //                                               (libusb_hotplug_event)((int)LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | (int)LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
    //                                               LIBUSB_HOTPLUG_ENUMERATE,
    //                                               0x1915,
    //                                               0x7777,
    //                                               LIBUSB_HOTPLUG_MATCH_ANY,
    //                                               hotplugCrazyradioCallback_C,
    //                                               this,
    //                                               /*libusb_hotplug_callback_handle*/ NULL);
    // if (result != LIBUSB_SUCCESS)
    // {
    //     throw std::runtime_error(libusb_error_name(result));
    // }

    // // register (&enumerate) all Crazyflies over USB
    // result = libusb_hotplug_register_callback(ctx_,
    //                                           (libusb_hotplug_event)((int)LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | (int)LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
    //                                           LIBUSB_HOTPLUG_ENUMERATE,
    //                                           0x0483,
    //                                           0x5740,
    //                                           LIBUSB_HOTPLUG_MATCH_ANY,
    //                                           hotplugCrazyflieUSBCallback_C,
    //                                           this,
    //                                           /*libusb_hotplug_callback_handle*/ NULL);
    // if (result != LIBUSB_SUCCESS)
    // {
    //     throw std::runtime_error(libusb_error_name(result));
    // }
}

USBManager::~USBManager()
{
    // std::cout << "dtor USBManager" << std::endl;
    // TODO: deregister hotplug callbacks

    crazyfliesUSB_.clear();
    radioThreads_.clear();

    // std::cout << "  deregister libusb devices" << std::endl;

    // deregister devices
    for (auto& t : radioThreads_) {
        libusb_unref_device(t.device());
    }
    for (auto dev : crazyfliesUSB_)
    {
        libusb_unref_device(dev);
    }

    // function returns void => no error checking
    libusb_exit(ctx_);
}



// void USBManager::hotplugCrazyradioCallback(libusb_device * /*dev*/,
//                                     libusb_hotplug_event event)
// {
//     switch (event)
//     {
//         case LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED:
//             std::cout << "Crazyradio detected!" << std::endl;
//             break;
//         case LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT:
//             std::cout << "Crazyradio left!" << std::endl;
//             break;
//     }
// }

// void USBManager::hotplugCrazyflieUSBCallback(libusb_device * /*dev*/,
//                                            libusb_hotplug_event event)
// {
//     switch (event)
//     {
//     case LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED:
//         std::cout << "CrazyflieUSB detected!" << std::endl;
//         break;
//     case LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT:
//         std::cout << "CrazyflieUSB left!" << std::endl;
//         break;
//     }
// }

void USBManager::addRadioConnection(std::shared_ptr<ConnectionImpl> connection)
{
    const std::lock_guard<std::mutex> lk(mutex_);

    int devId = connection->devid_;
    int channel = connection->channel_;
    auto datarate = connection->datarate_;

    // std::cout << "addCon " << connection->uri_ << std::endl;

    // assign a radio automatically
    if (devId == -1) {

        if (radioThreads_.empty()) {
            throw std::runtime_error("No Crazyradio dongle found!");
        }

        // GREEDY DOES NOT WORK
        // ex: 0/2M on radio 0, 2/1M on radio 1
        // try to schedule: 1/2M -> bam

        // a) i) check if a radio already has the same channel
        //   ii) if the datarate is 2M, also use consecutive channels

        std::map<int, std::vector<std::shared_ptr<ConnectionImpl>>> constraints;

        for (size_t i = 0; i < radioThreads_.size(); ++i) {
            auto& radioThread = radioThreads_[i];
            for (auto con : radioThread.connections_) {
                if (  (con->channel_ == channel)
                   || (con->datarate_ == Crazyradio::Datarate_2MPS && con->channel_+1 == channel)
                   || (datarate == Crazyradio::Datarate_2MPS && con->channel_ == channel + 1)) {
                    // std::cout << "use radio " << i << " b/c radio serves " << con->channel_ << " we need " << channel << std::endl;
                    // devId = i;
                    constraints[i].push_back(con);
                    // break;
                }
            }
            // if (devId >= 0) {
                // break;
            // }
        }

        // b) if no radio serves this channel, pick the one with the fewest
        //    connections

        if (constraints.empty()) {
            devId = 0;
            size_t min_connections_count = radioThreads_[0].connections_.size();
            for (size_t i = 1; i < radioThreads_.size(); ++i) {
                if (min_connections_count > radioThreads_[i].connections_.size()) {
                    devId = i;
                }
            }
            // std::cout << "use radio " << devId << " to serve " << channel << " (greedy)"  << std::endl;
        } else if (constraints.size() == 1) {
            // easy resolution -> constraints only from one radio
            devId = constraints.begin()->first;
        } else {
            // need to move some connections to resolve the conflict
            
            // assign to most constrained radio
            size_t max_constraints = 0;
            for (const auto& c : constraints) {
                if (c.second.size() > max_constraints) {
                    devId = c.first;
                    max_constraints = c.second.size();
                }
            }

            // move other connection to this radio, too
            for (const auto &c : constraints) {
                if (c.first != devId) {
                    for (auto con : c.second) {
                        radioThreads_[con->devid_].removeConnection(con);
                        con->devid_ = devId;
                        radioThreads_[devId].addConnection(con);
                    }
                }
            }
        }
    }
    assert(devId >= 0);

    if (devId < 0 || devId >= (int)radioThreads_.size()) {
        std::stringstream sstr;
        sstr << "No Crazyradio with id=" << devId << " found.";
        sstr << "There are " << radioThreads_.size() << " Crazyradios connected.";
        throw std::runtime_error(sstr.str());
    }

    // Sanity check (connections)
    for (auto& radioThread : radioThreads_) {
        for (auto con : radioThread.connections_) {
            if (   con->channel_ == connection->channel_
                && con->address_ == connection->address_ 
                && con->datarate_ == connection->datarate_) {
                std::stringstream sstr;
                sstr << "Connection " << connection->uri_ << " is using the same settings as an existing connection!";
                throw std::runtime_error(sstr.str());
            }
        }
    }

    // Sanity checks (radio assignment)
    for (size_t i = 0; i < radioThreads_.size(); ++i) {
        if ((int)i != devId) {
            auto &radioThread = radioThreads_[i];
            for (auto con : radioThread.connections_) {
                if (con->channel_ == channel) {
                    std::stringstream sstr;
                    sstr << "Channel " << channel << " is already served by Crazyradio " << i 
                         << ". Cannot be served by Crazyradio " << devId << " simultaneously." << con->uri_ << " " << connection->uri_;
                    throw std::runtime_error(sstr.str());
                }
                if (con->datarate_ == Crazyradio::Datarate_2MPS && con->channel_+1 == channel) {
                    std::stringstream sstr;
                    sstr << "Channels " << con->channel_ << " and " << con->channel_+1 << " are already served by Crazyradio " << i
                         << " (2M mode). Cannot be served by Crazyradio " << devId << " simultaneously.";
                    throw std::runtime_error(sstr.str());
                }
                if (datarate == Crazyradio::Datarate_2MPS && con->channel_ == channel + 1) {
                    std::stringstream sstr;
                    sstr << "Channel " << con->channel_ << " is already served by Crazyradio " << i
                         << ". Cannot be served by Crazyradio " << devId << " (2M mode) simultaneously.";
                    throw std::runtime_error(sstr.str());
                }
            }
        }
    }

    // add the connection to the radio
    connection->devid_ = devId;
    // std::cout << "new connection scheduled on " << channel << " " << devId << std::endl;
    radioThreads_[devId].addConnection(connection);
}

void USBManager::removeRadioConnection(std::shared_ptr<ConnectionImpl> con)
{
    const std::lock_guard<std::mutex> lk(mutex_);
    // std::cout << "rmCon " << con->uri_ << std::endl;
    radioThreads_[con->devid_].removeConnection(con);
    con->devid_ = -1;
}
