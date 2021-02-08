#include <iostream>

#include "CrazyflieUSBThread.h"
#include "CrazyflieUSB.h"
#include "ConnectionImpl.h"

CrazyflieUSBThread::CrazyflieUSBThread(libusb_device *dev)
    : dev_(dev)
    , thread_ending_(false)
{

}

CrazyflieUSBThread::~CrazyflieUSBThread()
{
    const std::lock_guard<std::mutex> lock(thread_mutex_);
    if (thread_.joinable()) {
        thread_.join();
    }
}

void CrazyflieUSBThread::addConnection(std::shared_ptr<ConnectionImpl> con)
{
    const std::lock_guard<std::mutex> lock(thread_mutex_);
    if (!thread_.joinable() && !connection_) {
        connection_ = con;
        thread_ = std::thread(&CrazyflieUSBThread::runWithErrorHandler, this);
    } else {
        throw std::runtime_error("Cannot operate more than one connection over USB!");
    }
}

void CrazyflieUSBThread::removeConnection(std::shared_ptr<ConnectionImpl> con)
{
    if (connection_ != con) {
        throw std::runtime_error("Connection does not belong to this thread!");
    }

    const std::lock_guard<std::mutex> lock(thread_mutex_);
    thread_ending_ = true;
    thread_.join();
    thread_ = std::thread();
    thread_ending_ = false;
    connection_.reset();
}

void CrazyflieUSBThread::runWithErrorHandler()
{
    try {
        run();
    }
    catch (const std::runtime_error &error) {
        connection_->runtime_error_ = error.what();
    }
    catch (...) {
    }
}

void CrazyflieUSBThread::run()
{
    CrazyflieUSB cf(dev_);

    while (!thread_ending_)
    {
        std::this_thread::yield();

        {
            const std::lock_guard<std::mutex> lock(connection_->queue_send_mutex_);
            if (!connection_->queue_send_.empty())
            {
                Packet p_send = connection_->queue_send_.top();
                bool success = cf.send(p_send.raw(), p_send.size());
                if (success) {
                    ++connection_->statistics_.sent_count;
                    connection_->queue_send_.pop();
                }
            }
        }

        Packet p_recv;
        size_t size = cf.recv(p_recv.raw(), CRTP_MAXSIZE, 1);
        p_recv.setSize(size);
        
        if (p_recv) {
            {
                const std::lock_guard<std::mutex> lock(connection_->queue_recv_mutex_);
                p_recv.seq_ = connection_->statistics_.receive_count;
                connection_->queue_recv_.push(p_recv);
                ++connection_->statistics_.receive_count;
            }
            connection_->queue_recv_cv_.notify_one();
        }
    }
}
