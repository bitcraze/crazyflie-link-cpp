#include <iostream>

#include "CrazyradioThread.h"
#include "Crazyradio.h"
// #include "native_link/Connection.h"
#include "ConnectionImpl.h"

namespace bitcraze {
namespace crazyflieLinkCpp {

CrazyradioThread::CrazyradioThread(libusb_device *dev)
    : dev_(dev)
    , thread_ending_(false)
{
    libusb_ref_device(dev_);
}

CrazyradioThread::CrazyradioThread(CrazyradioThread &&other)
{
    std::unique_lock<std::mutex> rhs_lk1(other.thread_mutex_, std::defer_lock);
    std::unique_lock<std::mutex> rhs_lk2(other.connections_mutex_, std::defer_lock);
    std::lock(rhs_lk1, rhs_lk2);
    dev_ = std::move(other.dev_);
    libusb_ref_device(dev_);
    thread_ = std::move(other.thread_);
    thread_ending_ = std::move(other.thread_ending_);
    connections_updated_ = std::move(other.connections_updated_);
    connections_ = std::move(other.connections_);
    runtime_error_ = std::move(other.runtime_error_);
}

CrazyradioThread::~CrazyradioThread()
{
    const std::lock_guard<std::mutex> lock(thread_mutex_);
    if (thread_.joinable()) {
        thread_.join();
    }
    libusb_unref_device(dev_);
}

// bool CrazyradioThread::isActive() const
// {
//     return thread_.joinable();
// }

void CrazyradioThread::addConnection(std::shared_ptr<ConnectionImpl> con)
{
    // bool startThread;
    {
        const std::lock_guard<std::mutex> lock(connections_mutex_);
        connections_.insert(con);
        if (!runtime_error_.empty()) {
            con->runtime_error_ = runtime_error_;
        }
        // startThread = thread_ended_;
        // std::cout << "add con " << connections_.size() << std::endl;
    }

    {
        const std::lock_guard<std::mutex> lock(thread_mutex_);
        if (!thread_.joinable()) {
            // std::cout << "add con " << startThread << std::endl;
            thread_ = std::thread(&CrazyradioThread::runWithErrorHandler, this);
        }
    }

    // if (startThread) {
    //     if (thread_.joinable()) {
    //         thread_.join();
    //     }
    //     {
    //         const std::lock_guard<std::mutex> lock(connections_mutex_);
    //         thread_ended_ = false;
    //     }
    //     thread_ = std::thread(&CrazyradioThread::run, this);
    // }
}

void CrazyradioThread::removeConnection(std::shared_ptr<ConnectionImpl> con)
{
    bool endThread;
    {
        std::unique_lock<std::mutex> lk(connections_mutex_);
        connections_updated_ = false;
        connections_.erase(con);
        endThread = connections_.empty();
        connections_updated_cv_.wait(lk, [this] { return !connections_updated_; });
    }

    if (endThread) {
        const std::lock_guard<std::mutex> lock(thread_mutex_);
        thread_ending_ = true;
        thread_.join();
        thread_ = std::thread();
        thread_ending_ = false;
    }
}

void CrazyradioThread::runWithErrorHandler()
{
    try {
        run();
    }
    catch (const std::runtime_error &error) {
        const std::lock_guard<std::mutex> lock(connections_mutex_);
        for (auto con : connections_) {
            std::lock(con->queue_send_mutex_, con->queue_recv_mutex_);
            std::lock_guard<std::mutex> lk1(con->queue_send_mutex_, std::adopt_lock);
            std::lock_guard<std::mutex> lk2(con->queue_recv_mutex_, std::adopt_lock);
            con->runtime_error_ = error.what();
        }
        runtime_error_ = error.what();
    }
    catch (...) {
    }
}

void CrazyradioThread::run()
{
    Crazyradio radio(dev_);

    const uint8_t enableSafelink[] = {0xFF, 0x05, 1};
    const uint8_t ping[] = {0xFF};

    std::set<std::shared_ptr<ConnectionImpl>> connections_copy;

    while (true)
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::this_thread::yield();

        // copy connections_
        bool thread_ending;
        {
            const std::lock_guard<std::mutex> lock(connections_mutex_);
            connections_copy = connections_;
            connections_updated_ = true;
            thread_ending = thread_ending_;
        }

        connections_updated_cv_.notify_one();
        if (thread_ending)
        {
            // std::cout << "ending..." << std::endl;
            break;
        }

        for (auto con : connections_copy) {
        // for (auto con : connections_) {
            // const std::lock_guard<std::mutex> con_lock(con->alive_mutex_);
            // if (!con->alive_) {
            //     continue;
            // }
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
                    ack = radio.sendPacket(enableSafelink, sizeof(enableSafelink));
                    ++con->statistics_.sent_count;
                    if (ack) {
                        con->safelinkInitialized_ = true;
                    }
                }
            }

            Packet p(ping, sizeof(ping));

            if (con->retry_) {
                p = con->retry_;
            } else {
                const std::lock_guard<std::mutex> lock(con->queue_send_mutex_);
                if (!con->queue_send_.empty())
                {
                    p = con->queue_send_.top();
                    con->queue_send_.pop();
                } else if (!con->useAutoPing_)
                {
                    continue;
                }
            }

            if (con->useSafelink_)
            {
                p.setSafelink(con->safelinkUp_ << 1 | con->safelinkDown_);
            }

            ack = radio.sendPacket(p.raw(), p.size());
            ++con->statistics_.sent_count;
            if (ack)
            {
                if (con->useSafelink_)
                {
                    con->safelinkUp_ = !con->safelinkUp_;
                }

                if (con->retry_) {
                    con->retry_ = Packet();
                }
            } else {
                con->retry_ = p;
            }

            if (con->useSafelink_)
            {
                if (ack && ack.size() > 0 && (ack.data()[0] & 0x04) == (con->safelinkDown_ << 2)) {
                    con->safelinkDown_ = !con->safelinkDown_;
                } else {
                    // reset ack to be invalid since safelink does not match
                    ack = Crazyradio::Ack();
                }
            }

            // enqueue result
            if (ack) {
                ++con->statistics_.ack_count;
                Packet p_ack(ack.data(), ack.size());
                if (con->useAckFilter_ &&
                    ack.size() == 0)
                    // p_ack.port() == 15 && p_ack.channel() == 3)
                {
                    // Empty packet -> update stats only
                    // con->statistics_.rssi_latest = p_ack.payload()[1];
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

} // namespace crazyflieLinkCpp
} // namespace bitcraze