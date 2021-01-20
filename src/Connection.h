#pragma once

#include <string>
#include <cstdint>
#include <cmath>
#include <ostream>
#include <vector>
#include <queue>
#include <mutex>

#include "Packet.hpp"
#include "Crazyradio.h"

class CrazyradioThread;

class Connection
{
  friend class CrazyradioThread;
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
  int channel_;
  Crazyradio::Datarate datarate_;
  uint64_t address_;
  bool useSafelink_;
  bool safelinkInitialized_;
  bool safelinkDown_;
  bool safelinkUp_;

  Connection::Statistics statistics_;

  std::mutex queue_send_mutex_;
  std::priority_queue<Packet> queue_send_;

  std::mutex queue_recv_mutex_;
  std::priority_queue<Packet> queue_recv_;
};
