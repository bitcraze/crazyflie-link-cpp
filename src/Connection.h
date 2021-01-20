#pragma once

#include <string>
#include <cstdint>
#include <cmath>
#include <ostream>
#include <vector>
#include <queue>

#include "Packet.hpp"

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
    
    void reset()
    {
      sent_count = 0;
      receive_count = 0;
      rssi_total = 0;
    }

    friend std::ostream &operator<<(std::ostream &out, const Connection::Statistics &s)
    {
      out << "Statistics(";
      out << "sent_count=" << s.sent_count;
      out << ",receive_count=" << s.receive_count;
      out << ",avg_rssi=" << ((s.receive_count > 0) ? (s.rssi_total / (double) s.receive_count) : std::nan(""));
      out << ")";

      return out;
    }

    size_t sent_count;
    size_t receive_count;
    size_t rssi_total;
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

  static std::vector<std::string> scan(const std::string& address);

  void send(const Packet& p);

  Packet recv(bool blocking);

  const std::string& uri() const
  {
    return uri_;
  }

  Connection::Statistics& statistics()
  {
    return statistics_;
  }

  friend std::ostream& operator<<(std::ostream& out, const Connection& p);

private:
  std::string uri_;
  Connection::Statistics statistics_;

  std::priority_queue<Packet> queue_send_;
  std::priority_queue<Packet> queue_recv_;
};
