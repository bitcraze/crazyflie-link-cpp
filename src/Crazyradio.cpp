#include "Crazyradio.h"

#include <sstream>
#include <stdexcept>

#include <libusb.h>

namespace bitcraze {
namespace crazyflieLinkCpp {

enum
{
    SET_RADIO_CHANNEL   = 0x01,
    SET_RADIO_ADDRESS   = 0x02,
    SET_DATA_RATE       = 0x03,
    SET_RADIO_POWER     = 0x04,
    SET_RADIO_ARD       = 0x05,
    SET_RADIO_ARC       = 0x06,
    ACK_ENABLE          = 0x10,
    SET_CONT_CARRIER    = 0x20,
    // SCANN_CHANNELS      = 0x21,
    LAUNCH_BOOTLOADER   = 0xFF,
};

Crazyradio::Crazyradio(
    libusb_device* dev)
    : USBDevice(dev)
    , channel_(0)
    , address_(0)
    , datarate_(Datarate_250KPS)
    , ackEnabled_(true)
{
    setDatarate(Datarate_2MPS);
    setChannel(2);
    setContCarrier(false);
    setAddress(0xE7E7E7E7E7);
    setPower(Power_0DBM);
    setArc(3);
    setArdBytes(32);
    setAckEnabled(true);
}

Crazyradio::~Crazyradio()
{
}

void Crazyradio::setChannel(uint8_t channel)
{
    sendVendorSetup(SET_RADIO_CHANNEL, channel, 0, NULL, 0);
    channel_ = channel;
}

void Crazyradio::setAddress(uint64_t address)
{
    unsigned char a[5];
    a[4] = (address >> 0) & 0xFF;
    a[3] = (address >> 8) & 0xFF;
    a[2] = (address >> 16) & 0xFF;
    a[1] = (address >> 24) & 0xFF;
    a[0] = (address >> 32) & 0xFF;

    sendVendorSetup(SET_RADIO_ADDRESS, 0, 0, a, 5);
    address_ = address;
}

void Crazyradio::setDatarate(Datarate datarate)
{
    sendVendorSetup(SET_DATA_RATE, datarate, 0, NULL, 0);
    datarate_ = datarate;
}

void Crazyradio::setPower(Power power)
{
    sendVendorSetup(SET_RADIO_POWER, power, 0, NULL, 0);
}

void Crazyradio::setArc(uint8_t arc)
{
    sendVendorSetup(SET_RADIO_ARC, arc, 0, NULL, 0);
}

void Crazyradio::setArdTime(uint8_t us)
{
    // Auto Retransmit Delay:
    // 0000 - Wait 250uS
    // 0001 - Wait 500uS
    // 0010 - Wait 750uS
    // ........
    // 1111 - Wait 4000uS

    // Round down, to value representing a multiple of 250uS
    int t = (us / 250) - 1;
    if (t < 0) {
        t = 0;
    }
    if (t > 0xF) {
        t = 0xF;
    }
    sendVendorSetup(SET_RADIO_ARD, t, 0, NULL, 0);
}

void Crazyradio::setArdBytes(uint8_t nbytes)
{
    sendVendorSetup(SET_RADIO_ARD, 0x80 | nbytes, 0, NULL, 0);
}

void Crazyradio::setAckEnabled(bool enable)
{
    sendVendorSetup(ACK_ENABLE, enable, 0, NULL, 0);
    ackEnabled_ = enable;
}

void Crazyradio::setContCarrier(bool active)
{
    sendVendorSetup(SET_CONT_CARRIER, active, 0, NULL, 0);
}

Crazyradio::Ack Crazyradio::sendPacket(
    const uint8_t* data,
    uint32_t length)
{
    Crazyradio::Ack ack;

    int status;
    int transferred;

    // Send data
    status = libusb_bulk_transfer(
        dev_handle_,
        /* endpoint*/ (0x01 | LIBUSB_ENDPOINT_OUT),
        (uint8_t*)data,
        length,
        &transferred,
        /*timeout*/ 100);
    // if (status == LIBUSB_ERROR_TIMEOUT) {
    //     return;
    // }
    if (status != LIBUSB_SUCCESS) {
        throw std::runtime_error(libusb_error_name(status));
    }
    if (length != (uint32_t)transferred) {
        std::stringstream sstr;
        sstr << "Did transfer " << transferred << " but " << length << " was requested!";
        throw std::runtime_error(sstr.str());
    }

    // Read result
    status = libusb_bulk_transfer(
        dev_handle_,
        /* endpoint*/ (0x81 | LIBUSB_ENDPOINT_IN),
        ack.data_.data(),
        ack.data_.size(),
        &transferred,
        /*timeout*/ 10);
    if (status == LIBUSB_ERROR_TIMEOUT) {
        return ack;
    }
    if (status != LIBUSB_SUCCESS) {
        throw std::runtime_error(libusb_error_name(status));
    }

    ack.size_ = transferred - 1;

    return ack;
}

void Crazyradio::sendPacketNoAck(
    const uint8_t* data,
    uint32_t length)
{
    int status;
    int transferred;

    // Send data
    status = libusb_bulk_transfer(
        dev_handle_,
        /* endpoint*/ (0x01 | LIBUSB_ENDPOINT_OUT),
        (uint8_t*)data,
        length,
        &transferred,
        /*timeout*/ 100);
    // if (status == LIBUSB_ERROR_TIMEOUT) {
    //     return;
    // }
    if (status != LIBUSB_SUCCESS) {
        throw std::runtime_error(libusb_error_name(status));
    }
    if (length != (uint32_t)transferred) {
        std::stringstream sstr;
        sstr << "Did transfer " << transferred << " but " << length << " was requested!";
        throw std::runtime_error(sstr.str());
    }
}

void Crazyradio::send2PacketsNoAck(
    const uint8_t* data,
    uint32_t totalLength)
{
    int status;
    int transferred;

    // Send data
    status = libusb_bulk_transfer(
        dev_handle_,
        /* endpoint*/ (0x01 | LIBUSB_ENDPOINT_OUT),
        (uint8_t*)data,
        totalLength,
        &transferred,
        /*timeout*/ 100);
    // if (status == LIBUSB_ERROR_TIMEOUT) {
    //     return;
    // }
    if (status != LIBUSB_SUCCESS) {
        throw std::runtime_error(libusb_error_name(status));
    }
    if (totalLength != (uint32_t)transferred) {
        std::stringstream sstr;
        sstr << "Did transfer " << transferred << " but " << totalLength << " was requested!";
        throw std::runtime_error(sstr.str());
    }
}

} // namespace crazyflieLinkCpp
} // namespace bitcraze