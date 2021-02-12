#pragma once

#include <stdint.h>
#include <vector>
#include <array>
#include <ostream>

#include "USBDevice.h"
#include "crazyflieLinkCpp/utils.hpp"

namespace bitcraze {
namespace crazyflieLinkCpp {

#define ACK_MAXSIZE 33

class Crazyradio
  : public USBDevice
{
public:
    enum Datarate
    {
        Datarate_250KPS = 0,
        Datarate_1MPS   = 1,
        Datarate_2MPS   = 2,
    };

    enum Power
    {
        Power_M18DBM = 0,
        Power_M12DBM = 1,
        Power_M6DBM  = 2,
        Power_0DBM   = 3,
    };

    class Ack
    {
        friend class Crazyradio;
    public:
        Ack()
            : data_()
            , size_(0)
        {
            data_[0] = 0;
        }

        bool ack() const
        {
            return getBitBieldValue<uint8_t, 0, 1>(data_[0]);
        }

        operator bool() const
        {
            return ack();
        }

        bool powerDet() const
        {
            return getBitBieldValue<uint8_t, 1, 1>(data_[0]);
        }

        uint8_t retry() const
        {
            return getBitBieldValue<uint8_t, 2, 4>(data_[0]);
        }

        const uint8_t* data() const
        {
            return &data_[1];
        }

        uint8_t size() const
        {
            return size_;
        }

        friend std::ostream &operator<<(std::ostream &out, const Ack &a)
        {
            out << "Ack(";
            out << "ack=" << a.ack();
            out << ",powerDet=" << a.powerDet();
            out << ",retry=" << (int)a.retry();
            out << ",data=";
            for (size_t i = 0; i < a.size_; ++i)
            {
                out << (int)a.data()[i] << " ";
            }
            out << ")";

            return out;
        }

    private:
        std::array<uint8_t, ACK_MAXSIZE> data_;
        size_t size_;
    };

public:
    Crazyradio(libusb_device *dev);

    virtual ~Crazyradio();

    // float version() const {
    //     return m_version;
    // }

    void setChannel(
        uint8_t channel);

    uint8_t channel() const {
        return channel_;
    }

    void setAddress(
        uint64_t address);

    uint64_t address() const {
        return address_;
    }

    void setDatarate(
        Datarate datarate);

    Datarate datarate() const {
        return datarate_;
    }

    void setPower(
        Power power);

    void setArc(
        uint8_t arc);

    void setArdTime(
        uint8_t us);

    void setArdBytes(
        uint8_t nbytes);

    void setAckEnabled(
        bool enable);

    bool ackEnabled() const {
        return ackEnabled_;
    }

    void setContCarrier(
        bool active);

    Ack sendPacket(
        const uint8_t* data,
        uint32_t length);

    void sendPacketNoAck(
        const uint8_t* data,
        uint32_t length);

    void send2PacketsNoAck(
        const uint8_t* data,
        uint32_t totalLength);

private:
    uint8_t channel_;
    uint64_t address_;
    Datarate datarate_;
    bool ackEnabled_;
};

} // namespace crazyflieLinkCpp
} // namespace bitcraze