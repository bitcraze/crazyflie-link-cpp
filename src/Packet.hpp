#pragma once

#include <cstdint>
#include <array>
#include <algorithm>

#define CRTP_MAXSIZE 31

class Packet
{
// private:
  // constexpr size_t CRTP_MAXSIZE() { return 31; }

public:
  Packet() = default;

  uint8_t channel:2;
  uint8_t link:2;
  uint8_t port:4;

  // use a fixed-size array to avoid dynamic allocation
  // at the cost of some memory overhead
  std::array<uint8_t, CRTP_MAXSIZE> data;
  // uint8_t data[CRTP_MAXSIZE];

  // actual size of data
  size_t size;

  friend std::ostream& operator<<(std::ostream& out, const Packet& p)
  {
    out << "Packet(";
    out << "channel=" << (int)p.channel;
    out << ",port=" << (int)p.port;
    out << ",data=";
    for (size_t i = 0; i < std::min<size_t>(CRTP_MAXSIZE, p.size); ++i) {
      out << p.data[i];
    }
    out << (int)p.data[0] << ")";

    return out;
  }
};
