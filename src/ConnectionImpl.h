#pragma once

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "Crazyradio.h"
#include "CrazyflieUSB.h"
#include "crazyflieLinkCpp/Connection.h"

namespace bitcraze {
namespace crazyflieLinkCpp {

class ConnectionImpl
{
public:
    std::string uri_;
    int devid_;

    Connection::Statistics statistics_;
    Connection::Statistics statistics_previous_;

    bool isRadio_;

    // Radio related
    int channel_;
    Crazyradio::Datarate datarate_;
    uint64_t address_;
    bool useSafelink_;
    bool useAutoPing_;
    bool useAckFilter_;
    bool safelinkInitialized_;
    bool safelinkDown_;
    bool safelinkUp_;
    bool broadcast_;

    std::mutex queue_send_mutex_;
    std::priority_queue<Packet, std::vector<Packet>, std::greater<Packet>> queue_send_;
    Packet retry_;

    std::mutex queue_recv_mutex_;
    std::condition_variable queue_recv_cv_;
    std::priority_queue<Packet, std::vector<Packet>, std::greater<Packet>> queue_recv_;

    std::string runtime_error_;
};

} // namespace crazyflieLinkCpp
} // namespace bitcraze