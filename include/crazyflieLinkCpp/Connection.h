#pragma once

#include <atomic>
#include <string>
#include <cstdint>
#include <cmath>
#include <ostream>
#include <vector>
#include <memory>
#include <limits>
#include "Packet.hpp"

#define BROADCAST_ADDRESS 0xFFE7E7E7E7

namespace bitcraze {
namespace crazyflieLinkCpp {

class ConnectionImpl;

class Connection
{
public:
  class Statistics
  {
  public:
    Statistics()
    {
      reset();
    }

    Statistics(const Statistics& other)
    {
      *this = other;
    }

    Statistics& operator=(const Statistics& other)
    {
      std::atomic_store(&sent_count, std::atomic_load(&other.sent_count));
      std::atomic_store(&sent_ping_count, std::atomic_load(&other.sent_ping_count));
      std::atomic_store(&receive_count, std::atomic_load(&other.receive_count));
      std::atomic_store(&enqueued_count, std::atomic_load(&other.enqueued_count));
      std::atomic_store(&ack_count, std::atomic_load(&other.ack_count));      
      return *this;
    }
    
    void reset()
    {
      sent_count = 0;
      sent_ping_count = 0;
      receive_count = 0;
      enqueued_count = 0;
      ack_count = 0;
      // rssi_latest = 0;
    }

    friend std::ostream &operator<<(std::ostream &out, const Connection::Statistics &s)
    {
      out << "Statistics(";
      out << "sent_count=" << s.sent_count;
      out << ",sent_ping_count=" << s.sent_ping_count;
      out << ",receive_count=" << s.receive_count;
      out << ",enqueued_count=" << s.enqueued_count;
      out << ",ack_count=" << s.ack_count;
      // out << ",rssi_latest=" << (int)s.rssi_latest;
      out << ")";

      return out;
    }

    friend Statistics operator-(const Statistics& lhs, const Statistics& rhs)
    {
      Statistics result;
      result.sent_count = lhs.sent_count - rhs.sent_count;
      result.sent_ping_count = lhs.sent_ping_count - rhs.sent_ping_count;
      result.receive_count = lhs.receive_count - rhs.receive_count;
      result.enqueued_count = lhs.enqueued_count - rhs.enqueued_count;
      result.ack_count = lhs.ack_count - rhs.ack_count;

      return result;
    }

    std::atomic_size_t sent_count;
    std::atomic_size_t sent_ping_count;
    std::atomic_size_t receive_count;
    std::atomic_size_t enqueued_count;
    std::atomic_size_t ack_count;
    // uint8_t rssi_latest;
  };

public:

  // non construction-copyable
  Connection(const Connection&) = delete;

  // non copyable
  Connection& operator=(const Connection&) = delete;

  // construct, given a URI, e.g. "radio://*/80/2M/*"
  Connection(const std::string& uri);

  // deconstruct/cleanup
  ~Connection();

  static std::vector<std::string> scan(uint64_t address = 0xE7E7E7E7E7);
  static std::vector<std::string> scan_selected(const std::vector<std::string>& uris);

  void send(const Packet& p);

  // Updated receive function that can be non-blocking, fully blocking, or use a timeout
  static constexpr unsigned int TimeoutNone = 0;
  static constexpr unsigned int TimeoutBlock = std::numeric_limits<unsigned int>::max();
  Packet receive(unsigned int timeout_in_ms);

  // Deprecated receive that can be fully blocking (timeout_in_ms == 0), or use a timeout
  Packet recv(unsigned int timeout_in_ms);

  void close();

  const std::string& uri() const;

  const Connection::Statistics statistics() const;

  const Connection::Statistics statisticsDelta();

  friend std::ostream& operator<<(std::ostream& out, const Connection& p);

private:
  std::shared_ptr<ConnectionImpl> impl_;
};

} // namespace crazyflieLinkCpp
} // namespace bitcraze