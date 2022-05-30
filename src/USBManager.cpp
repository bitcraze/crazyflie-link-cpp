#include "USBManager.h"

#include <stdexcept>
#include <functional>
#include <iostream>
#include <cassert>
#include <sstream>
#include <map>

// #include "native_link/Connection.h"
#include "ConnectionImpl.h"

namespace bitcraze {
namespace crazyflieLinkCpp {

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
    : crazyfliesUSB_lastDevId_(0)
    , crazyradios_lastDevId_(0)
{
    int result = libusb_init(&ctx_);
    if (result != LIBUSB_SUCCESS)
    {
        throw std::runtime_error(libusb_error_name(result));
    }

    updateDevices();

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
    // Fixes a crash where device gets destroyed before it gets unref'ed
    crazyradios_.clear();
    crazyfliesUSB_.clear();

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

std::string USBManager::tryToQuerySerialNumber(libusb_device *dev, const libusb_device_descriptor* deviceDescriptor)
{
    libusb_device_handle *dev_handle;
    int err = libusb_open(dev, &dev_handle);
    if (err != LIBUSB_SUCCESS)
    {
        return std::string();
    }

    err = libusb_claim_interface(dev_handle, 0);
    if (err != LIBUSB_SUCCESS)
    {
        libusb_close(dev_handle);
        return std::string();
    }

    unsigned char str[256];
    int length;
    length = libusb_get_string_descriptor_ascii(
        dev_handle, deviceDescriptor->iSerialNumber, str, sizeof(str));
    std::string serialNumber;
    if (length > 0) {
        serialNumber = std::string(reinterpret_cast<char *>(str), length);
    }
    libusb_release_interface(dev_handle, 0);
    libusb_close(dev_handle);
    return serialNumber;
}

void USBManager::updateDevices()
{
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
                auto sn = tryToQuerySerialNumber(device, &deviceDescriptor);
                if (!sn.empty()) {
                    auto iter = crazyfliesUSB_devIdbySN_.find(sn);
                    if (iter == crazyfliesUSB_devIdbySN_.end()) {
                        // newly detected -> assign a new devId
                        crazyfliesUSB_devIdbySN_[sn] = crazyfliesUSB_lastDevId_;
                        crazyfliesUSB_.emplace(std::make_pair(crazyfliesUSB_lastDevId_, CrazyflieUSBThread(device)));
                        ++crazyfliesUSB_lastDevId_;
                    } else {
                        auto iter2 = crazyfliesUSB_.find(iter->second);
                        if (iter2 == crazyfliesUSB_.end()) {
                            // a device that we have seen previously re-emerged
                            crazyfliesUSB_.emplace(std::make_pair(iter->second, CrazyflieUSBThread(device)));
                        }
                    }
                }
            }
            // Crazyradio
            else if (deviceDescriptor.idVendor == 0x1915 &&
                     deviceDescriptor.idProduct == 0x7777)
            {
                auto sn = tryToQuerySerialNumber(device, &deviceDescriptor);
                if (!sn.empty()) {
                    auto iter = crazyradios_devIdbySN_.find(sn);
                    if (iter == crazyradios_devIdbySN_.end()) {
                        // newly detected -> assign a new devId
                        crazyradios_devIdbySN_[sn] = crazyradios_lastDevId_;
                        crazyradios_.emplace(std::make_pair(crazyradios_lastDevId_, CrazyradioThread(device)));
                        // std::cout << "added new " << sn << " " << crazyradios_lastDevId_ << std::endl;
                        ++crazyradios_lastDevId_;
                    } else {
                        auto iter2 = crazyradios_.find(iter->second);
                        if (iter2 == crazyradios_.end()) {
                            // a device that we have seen previously re-emerged
                            crazyradios_.emplace(std::make_pair(iter->second, CrazyradioThread(device)));
                            // std::cout << "added old " << sn << " " << iter->second << std::endl;
                        }
                    }
                }
            }
        }
    }
    // function returns void => no error checking
    libusb_free_device_list(list, 1);

    // 2. remove all devices that caused an error.
    //    Do not delete devices that we couldn't enumerate, because they might be still in use.
    //    Keep the mapping of sn -> devid for future connections

    std::set<size_t> to_erase;
    for (const auto &iter : crazyfliesUSB_) {
        if (iter.second.hasError()) {
            to_erase.insert(iter.first);
        }
    }
    for (size_t devid : to_erase) {
        crazyfliesUSB_.erase(devid);
    }

    to_erase.clear();
    for (const auto& iter : crazyradios_) {
        if (iter.second.hasError()) {
            to_erase.insert(iter.first);
        }
    }
    for (size_t devid : to_erase) {
        crazyradios_.erase(devid);
        // std::cout << "rm " << devid << std::endl;
    }
}

void USBManager::addConnection(std::shared_ptr<ConnectionImpl> connection)
{
    const std::lock_guard<std::mutex> lk(mutex_);

    // ToDo: this is terrible for perf
    // updateDevices();

    if (!connection->isRadio_) {
        crazyfliesUSB_.at(connection->devid_).addConnection(connection);
        return;
    }

    int devId = connection->devid_;
    int channel = connection->channel_;
    auto datarate = connection->datarate_;

    // std::cout << "addCon " << connection->uri_ << std::endl;

    // assign a radio automatically
    if (devId == -1) {

        if (crazyradios_.empty()) {
            throw std::runtime_error("No Crazyradio dongle found!");
        }

        // GREEDY DOES NOT WORK
        // ex: 0/2M on radio 0, 2/1M on radio 1
        // try to schedule: 1/2M -> bam

        // a) i) check if a radio already has the same channel
        //   ii) if the datarate is 2M, also use consecutive channels

        std::map<int, std::vector<std::shared_ptr<ConnectionImpl>>> constraints;

        for (const auto& iter : crazyradios_) {
            size_t i = iter.first;
            auto& radioThread = iter.second;
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
            size_t min_connections_count = std::numeric_limits<size_t>::max();
            for (const auto& iter : crazyradios_) {
                if (min_connections_count > iter.second.connections_.size()) {
                    devId = iter.first;
                    min_connections_count = iter.second.connections_.size();
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
                        crazyradios_.at(con->devid_).removeConnection(con);
                        con->devid_ = devId;
                        crazyradios_.at(devId).addConnection(con);
                    }
                }
            }
        }
    }
    assert(devId >= 0);

    if (devId < 0 || devId >= (int)crazyradios_.size()) {
        std::stringstream sstr;
        sstr << "No Crazyradio with id=" << devId << " found.";
        sstr << "There are " << crazyradios_.size() << " Crazyradios connected.";
        throw std::runtime_error(sstr.str());
    }

    // Sanity check (connections)
    for (auto& radioThread : crazyradios_) {
        for (auto con : radioThread.second.connections_) {
            if (   con->channel_ == connection->channel_
                && con->address_ == connection->address_ 
                && con->datarate_ == connection->datarate_
                && con->broadcast_ == false && connection->broadcast_ == false) {
                std::stringstream sstr;
                sstr << "Connection " << connection->uri_ << " is using the same settings as an existing connection!";
                throw std::runtime_error(sstr.str());
            }
        }
    }

    // Sanity checks (radio assignment)
    for (const auto& iter : crazyradios_) {
        if ((int)iter.first != devId) {
            const auto& radioThread = iter.second;
            for (auto con : radioThread.connections_) {
                if (con->channel_ == channel) {
                    std::stringstream sstr;
                    sstr << "Channel " << channel << " is already served by Crazyradio " << iter.first 
                         << ". Cannot be served by Crazyradio " << devId << " simultaneously." << con->uri_ << " " << connection->uri_;
                    throw std::runtime_error(sstr.str());
                }
                if (con->datarate_ == Crazyradio::Datarate_2MPS && con->channel_+1 == channel) {
                    std::stringstream sstr;
                    sstr << "Channels " << con->channel_ << " and " << con->channel_+1 << " are already served by Crazyradio " << iter.first
                         << " (2M mode). Cannot be served by Crazyradio " << devId << " simultaneously.";
                    throw std::runtime_error(sstr.str());
                }
                if (datarate == Crazyradio::Datarate_2MPS && con->channel_ == channel + 1) {
                    std::stringstream sstr;
                    sstr << "Channel " << con->channel_ << " is already served by Crazyradio " << iter.first
                         << ". Cannot be served by Crazyradio " << devId << " (2M mode) simultaneously.";
                    throw std::runtime_error(sstr.str());
                }
            }
        }
    }

    // add the connection to the radio
    connection->devid_ = devId;
    // std::cout << "new connection scheduled on " << channel << " " << devId << std::endl;
    crazyradios_.at(devId).addConnection(connection);
}

void USBManager::removeConnection(std::shared_ptr<ConnectionImpl> con)
{
    const std::lock_guard<std::mutex> lk(mutex_);
    // std::cout << "rmCon " << con->uri_ << std::endl;
    if (con->isRadio_) {
        crazyradios_.at(con->devid_).removeConnection(con);
    } else {
        crazyfliesUSB_.at(con->devid_).removeConnection(con);
    }
    con->devid_ = -1;
}

} // namespace crazyflieLinkCpp
} // namespace bitcraze