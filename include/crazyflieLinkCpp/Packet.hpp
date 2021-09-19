#pragma once

#include <cstdint>
#include <cstring>
#include <array>
#include <algorithm>
#include <tuple>
#include <sstream>

#include "utils.hpp"

namespace bitcraze {
namespace crazyflieLinkCpp {

#define CRTP_MAXSIZE 32

class Connection;
class CrazyradioThread;
class CrazyflieUSBThread;

class Packet
{
// private:
  // constexpr size_t CRTP_MAXSIZE() { return 31; }
  friend class Connection;
  friend class CrazyradioThread;
  friend class CrazyflieUSBThread;

public:
  Packet() 
    : size_(0)
    , seq_(0)
  {
    data_[0] = 0xFF;
  }

  Packet(const uint8_t* data, size_t size)
  {
    std::memcpy(data_.data(), data, size);
    size_ = size;
    seq_ = 0;
    if (size_ == 0) {
      data_[0] = 0xFF;
      size_ = 1;
    }
  }

  Packet(uint8_t port, uint8_t channel, uint8_t payloadSize)
  {
    setPort(port);
    setChannel(channel);
    setPayloadSize(payloadSize);
    seq_ = 0;
  }

  bool valid() const
  {
    return size_ >= 1;
  }

  operator bool() const
  {
    return valid();
  }

  uint8_t channel() const
  {
    return getBitBieldValue<uint8_t, 0, 2>(data_[0]);
  }

  void setChannel(uint8_t channel)
  {
    setBitBieldValue<uint8_t, 0, 2>(data_[0], channel);
  }

  uint8_t port() const
  {
    return getBitBieldValue<uint8_t, 4, 4>(data_[0]);
  }

  void setPort(uint8_t port)
  {
    setBitBieldValue<uint8_t, 4, 4>(data_[0], port);
  }

  size_t payloadSize() const
  {
    return size_ == 0 ? 0 : size_ - 1;
  }

  void setPayloadSize(size_t size)
  {
    setSize(size + 1);
  }

  size_t size() const
  {
    return size_;
  }

  void setSize(size_t size)
  {
    if (size > CRTP_MAXSIZE) {
      std::stringstream sstr;
      sstr << "Size " << size << " too large! Maximum: " << CRTP_MAXSIZE << ".";
      throw std::runtime_error(sstr.str());
    }
    size_ = size;
  }

  uint8_t* payload()
  {
    return &data_[1];
  }

  const uint8_t *payload() const
  {
    return &data_[1];
  }

  void setPayloadAt(uint8_t idx, const uint8_t* data, size_t length)
  {
    memcpy(&payload()[idx], data, length);
  }

  template<class T>
  void setPayloadAt(uint8_t idx, const T& data)
  {
    memcpy(&payload()[idx], &data, sizeof(T));
  }

  template <class T>
  T payloadAt(uint8_t idx) const
  {
    T data;
    memcpy(&data, &payload()[idx], sizeof(T));
    return data;
  }

  // Only C++ 17 would allow to overload payloadAt properly
  // Use this version with a different name instead
  std::string payloadAtString(uint8_t idx) const
  {
    auto c_str = (const char *)&payload()[idx];
    size_t length = strnlen(c_str, payloadSize() - idx);
    std::string str(c_str, length);
    return str;
  }

  void setPayloadAtString(uint8_t idx, const std::string& data)
  {
    memcpy(&payload()[idx], data.c_str(), data.size()+1);
  }

  const uint8_t* raw() const
  {
    return &data_[0];
  }

  uint8_t *raw()
  {
    return &data_[0];
  }

  friend std::ostream& operator<<(std::ostream& out, const Packet& p)
  {
    out << "Packet(";
    out << "channel=" << (int)p.channel();
    out << ",port=" << (int)p.port();
    out << ",safelinkUp=" << p.safelinkUp();
    out << ",safelinkDown=" << p.safelinkDown();
    out << ",data=";
    for (size_t i = 1; i < p.size_; ++i) {
      out << (int)p.data_[i] << " ";
    }
    out << ",seq=" << p.seq_;
    out << ")";

    return out;
  }

  friend bool operator<(const Packet &l, const Packet &r)
  {
    return std::forward_as_tuple(l.port(), l.seq_) < std::forward_as_tuple(r.port(), r.seq_);
  }

  friend bool operator>(const Packet &l, const Packet &r)
  {
    return std::forward_as_tuple(l.port(), l.seq_) > std::forward_as_tuple(r.port(), r.seq_);
  }

private:
  bool safelinkUp() const
  {
    return getBitBieldValue<uint8_t, 3, 1>(data_[0]);
  }

  bool safelinkDown() const
  {
    return getBitBieldValue<uint8_t, 2, 1>(data_[0]);
  }

  uint8_t safelink() const
  {
    return getBitBieldValue<uint8_t, 2, 2>(data_[0]);
  }

  void setSafelink(uint8_t safelink)
  {
    setBitBieldValue<uint8_t, 2, 2>(data_[0], safelink);
  }

private:
  // use a fixed-size array to avoid dynamic allocation
  std::array<uint8_t, CRTP_MAXSIZE> data_;

  // actual size of data
  size_t size_;

  // sequence number for strict weak ordering
  mutable size_t seq_;
};

} // namespace crazyflieLinkCpp
} // namespace bitcraze