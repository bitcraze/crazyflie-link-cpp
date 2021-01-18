#pragma once

#include <cstdint>
#include <array>
#include <algorithm>

#define CRTP_MAXSIZE 31

template <class T, size_t Index, size_t Bits = 1>
void setBitBieldValue(T& ref, T new_val)
{
  T mask = (1u << Bits) - 1u;
  ref = (ref & ~(mask << Index)) | ((new_val & mask) << Index);
}

template <class T, size_t Index, size_t Bits = 1>
T getBitBieldValue(const T &ref)
{
  T mask = (1u << Bits) - 1u;
  return (ref >> Index) & mask;
}

class Packet
{
// private:
  // constexpr size_t CRTP_MAXSIZE() { return 31; }

public:
  Packet() 
    : size_(0)
  {
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

  uint8_t size() const
  {
    return size_;
  }

  void setSize(size_t size)
  {
    size_ = std::min<size_t>(CRTP_MAXSIZE, size);
  }

  uint8_t* data()
  {
    return &data_[1];
  }

  friend std::ostream& operator<<(std::ostream& out, const Packet& p)
  {
    out << "Packet(";
    out << "channel=" << (int)p.channel();
    out << ",port=" << (int)p.port();
    out << ",data=";
    for (size_t i = 1; i < p.size_+1; ++i) {
      out << (int)p.data_[i] << " ";
    }
    out << ")";

    return out;
  }

private:
  // use a fixed-size array to avoid dynamic allocation
  std::array<uint8_t, CRTP_MAXSIZE+1> data_;

  // actual size of data
  size_t size_;
};
