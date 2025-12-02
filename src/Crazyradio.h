#pragma once

#include <stdint.h>
#include <vector>
#include <array>
#include <ostream>

#include "USBDevice.h"
#include "crazyflieLinkCpp/utils.hpp"

namespace bitcraze {
namespace crazyflieLinkCpp {

#define ACK_MAXSIZE 34

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
            , inline_mode_(false)
        {
            data_[0] = 0;
        }

        bool ack() const
        {
            if (!inline_mode_) {
                return getBitBieldValue<uint8_t, 0, 1>(data_[0]);
            } else {
                return getBitBieldValue<uint8_t, 0, 1>(data_[1]);
            }
        }

        operator bool() const
        {
            return ack();
        }

        bool powerDet() const
        {
            if (!inline_mode_) {
                return getBitBieldValue<uint8_t, 1, 1>(data_[0]);
            } else {
                return getBitBieldValue<uint8_t, 1, 1>(data_[1]);
            }
        }

        bool invalidSettings() const {
            if (!inline_mode_) {
                return false;
            } else {
                return getBitBieldValue<uint8_t, 2, 2>(data_[1]);
            }
        }

        uint8_t retry() const
        {
            if (!inline_mode_) {
                return getBitBieldValue<uint8_t, 2, 4>(data_[0]);
            } else {
                return getBitBieldValue<uint8_t, 4, 7>(data_[1]);
            }
        }

        const uint8_t* data() const
        {
            if (!inline_mode_) {
                return &data_[1];
            } else {
                return &data_[2];
            }
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
        bool inline_mode_;
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

    void setInlineMode(
        bool enable);

    bool ackEnabled() const {
        return ackEnabled_;
    }

    void setContCarrier(
        bool active);

    // only available for Crazyradio 2.0 with firmware 5.0 and greater
    void setPacketLossSimulation(
        uint8_t packet_loss_percent,
        uint8_t ack_loss_percent);
    
    Ack sendPacket(
        const uint8_t* data,
        uint32_t length);

    void sendPacketNoAck(
        const uint8_t* data,
        uint32_t length);

    void send2PacketsNoAck(
        const uint8_t* data,
        uint32_t totalLength);

    Ack sendPacketInline(
        const uint8_t* data,
        uint32_t length,
        Datarate datarate,
        uint8_t channel,
        uint64_t address,
        bool ackEnabled);

private:
    uint8_t channel_;
    uint64_t address_;
    Datarate datarate_;
    bool ackEnabled_;
};

} // namespace crazyflieLinkCpp
} // namespace bitcraze