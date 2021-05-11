#pragma once

#include <atomic>
#include <string>
#include <cstdint>
#include <cmath>
#include <ostream>
#include <vector>
#include <memory>

#include "Packet.hpp"

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
      std::atomic_store(&sent_count, std::atomic_load(&other.sent_count));
      std::atomic_store(&receive_count, std::atomic_load(&other.receive_count));
      std::atomic_store(&enqueued_count, std::atomic_load(&other.enqueued_count));
      std::atomic_store(&ack_count, std::atomic_load(&other.ack_count));
    }
    
    void reset()
    {
      sent_count = 0;
      receive_count = 0;
      enqueued_count = 0;
      ack_count = 0;
      // rssi_latest = 0;
    }

    friend std::ostream &operator<<(std::ostream &out, const Connection::Statistics &s)
    {
      out << "Statistics(";
      out << "sent_count=" << s.sent_count;
      out << ",receive_count=" << s.receive_count;
      out << ",enqueued_count=" << s.enqueued_count;
      out << ",ack_count=" << s.ack_count;
      // out << ",rssi_latest=" << (int)s.rssi_latest;
      out << ")";

      return out;
    }

    std::atomic_size_t sent_count;
    std::atomic_size_t receive_count;
    std::atomic_size_t enqueued_count;
    std::atomic_size_t ack_count;
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

  Packet recv(unsigned int timeout_in_ms);

  void close();

  const std::string& uri() const;

  Connection::Statistics statistics();

  friend std::ostream& operator<<(std::ostream& out, const Connection& p);

private:
  std::shared_ptr<ConnectionImpl> impl_;
};

} // namespace crazyflieLinkCpp
} // namespace bitcraze