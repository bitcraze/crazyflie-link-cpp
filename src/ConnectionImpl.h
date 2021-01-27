#pragma once

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "Crazyradio.h"
#include "native_link/Connection.h"

class ConnectionImpl
{
public:
    std::string uri_;
    int devid_;
    int channel_;
    Crazyradio::Datarate datarate_;
    uint64_t address_;
    bool useSafelink_;
    bool safelinkInitialized_;
    bool safelinkDown_;
    bool safelinkUp_;

    Connection::Statistics statistics_;

    std::mutex queue_send_mutex_;
    std::priority_queue<Packet> queue_send_;

    std::mutex queue_recv_mutex_;
    std::condition_variable queue_recv_cv_;
    std::priority_queue<Packet> queue_recv_;
};