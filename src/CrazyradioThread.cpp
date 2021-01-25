#include <iostream>

#include "CrazyradioThread.h"
#include "Crazyradio.h"
#include "Connection.h"

CrazyradioThread::CrazyradioThread(libusb_device *dev)
    : dev_(dev)
{

}

CrazyradioThread::~CrazyradioThread()
{
    if (isActive()) {
        thread_.join();
    }
}

bool CrazyradioThread::isActive() const
{
    return thread_.joinable();
}

void CrazyradioThread::addConnection(Connection *con)
{
    const std::lock_guard<std::mutex> lock(connections_mutex_);
    connections_.insert(con);
    if (!isActive()) {
        thread_ = std::thread(&CrazyradioThread::run, this);
    }
}

void CrazyradioThread::removeConnection(Connection *con)
{
    const std::lock_guard<std::mutex> lock(connections_mutex_);
    connections_.erase(con);
}

void CrazyradioThread::run()
{
    Crazyradio radio(dev_);

    const uint8_t enableSafelink[] = {0xFF, 0x05, 1};
    const uint8_t ping[] = {0xFF};

    std::set<Connection *> connections_copy;

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // copy connections_
        {
            const std::lock_guard<std::mutex> lock(connections_mutex_);
            connections_copy = connections_;
        }
        if (connections_copy.empty()) {
            break;
        }

        for (auto con : connections_copy) {
            const std::lock_guard<std::mutex> con_lock(con->alive_mutex_);
            if (!con->alive_) {
                continue;
            }
            // reconfigure radio if needed
            if (radio.address() != con->address_)
            {
                radio.setAddress(con->address_);
            }
            if (radio.channel() != con->channel_)
            {
                radio.setChannel(con->channel_);
            }
            if (radio.datarate() != con->datarate_)
            {
                radio.setDatarate(con->datarate_);
            }
            if (!radio.ackEnabled())
            {
                radio.setAckEnabled(true);
            }

            // prepare to send result
            Crazyradio::Ack ack;

            // initialize safelink if needed
            if (con->useSafelink_) {
                if (!con->safelinkInitialized_) {
                    auto ack = radio.sendPacket(enableSafelink, sizeof(enableSafelink));
                    if (ack) {
                        con->safelinkInitialized_ = true;
                    }
                } else {
                    // send actual packet via safelink

                    const std::lock_guard<std::mutex> lock(con->queue_send_mutex_);
                    Packet p(ping, sizeof(ping));
                    if (!con->queue_send_.empty())
                    {
                        p = con->queue_send_.top();
                    }

                    p.setSafelink(con->safelinkUp_ << 1 | con->safelinkDown_);
                    ack = radio.sendPacket(p.raw(), p.size() + 1);
                    if (ack && ack.size() > 0 && (ack.data()[0] & 0x04) == (con->safelinkDown_ << 2)) {
                        con->safelinkDown_ = !con->safelinkDown_;
                    }
                    if (ack)
                    {
                        con->safelinkUp_ = !con->safelinkUp_;
                        if (!con->queue_send_.empty()) {
                            con->queue_send_.pop();
                        }
                    }
                }
            } else {
                // no safelink
                const std::lock_guard<std::mutex> lock(con->queue_send_mutex_);
                if (!con->queue_send_.empty())
                {
                    const auto p = con->queue_send_.top();
                    ack = radio.sendPacket(p.raw(), p.size() + 1);
                    if (ack)
                    {
                        con->queue_send_.pop();
                    }
                }
                else
                {
                    ack = radio.sendPacket(ping, sizeof(ping));
                }
            }

            // enqueue result
            if (ack) {
                Packet p_ack(ack.data(), ack.size());
                if (p_ack.port() == 15 && p_ack.channel() == 3)
                {
                    // Empty packet -> update stats only
                    con->statistics_.rssi_latest = p_ack.data()[1];
                }
                else
                {
                    {
                        const std::lock_guard<std::mutex> lock(con->queue_recv_mutex_);
                        p_ack.seq_ = con->statistics_.receive_count;
                        con->queue_recv_.push(p_ack);
                        ++con->statistics_.receive_count;
                    }
                    con->queue_recv_cv_.notify_one();
                }
            }
        }
    }
}
