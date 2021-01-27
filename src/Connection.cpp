#include <regex>
#include <iostream>
#include <future>

#include "native_link/Connection.h"

#include "USBManager.h"
#include "Crazyradio.h"

#include <libusb-1.0/libusb.h>

Connection::Connection(const std::string &uri, const Connection::Settings &settings)
    : uri_(uri)
{
  // Examples:
  // "usb://0" -> connect over USB
  // "radio://0/80/2M/E7E7E7E7E7" -> connect over radio
  // "radio://*/80/2M/E7E7E7E7E7" -> auto-pick radio
  // "radio://*/80/2M/*" -> broadcast/P2P sniffing on channel 80

  const std::regex uri_regex("(usb:\\/\\/(\\d+)|radio:\\/\\/(\\d+|\\*)\\/(\\d+)\\/(250K|1M|2M)\\/([a-fA-F0-9]{10}|\\*))");
  std::smatch match;
  if (!std::regex_match(uri, match, uri_regex)) {
    throw std::runtime_error("Invalid uri!");
  }

  // for (size_t i = 0; i < match.size(); ++i) {
  //   std::cout << i << " " << match[i].str() << std::endl;
  // }
  // std::cout << match.size() << std::endl;

  if (match[2].matched) {
    // usb://
    // int devid = std::stoi(match[2].str());
    // TODO
  } else {
    // radio
    if (match[3].str() == "*") {
      devid_ = -1;
    } else {
      devid_ = std::stoi(match[3].str());
    }

    channel_ = std::stoi(match[4].str());
    if (match[5].str() == "250K") {
      datarate_ = Crazyradio::Datarate_250KPS;
    } else if (match[5].str() == "1M") {
      datarate_ = Crazyradio::Datarate_1MPS;
    } else if (match[5].str() == "2M") {
      datarate_ = Crazyradio::Datarate_2MPS;
    }
    address_ = std::stoul(match[6].str(), nullptr, 16);
    useSafelink_ = settings.use_safelink;
    safelinkInitialized_ = false;
    safelinkUp_ = false;
    safelinkDown_ = false;

    alive_ = true;

    USBManager::get().addConnection(this);

  }

}

Connection::~Connection()
{
  USBManager::get().removeConnection(this);
  // {
  //   const std::lock_guard<std::mutex> lock(alive_mutex_);
  //   alive_ = false;
  // }
}

std::vector<std::string> Connection::scan(const std::string& address)
{
  std::vector<std::string> result;

  // Crazyflies over USB
  for (size_t i = 0; i < USBManager::get().numCrazyfliesOverUSB(); ++i) {
    result.push_back("usb://" + std::to_string(i));
  }

  // Crazyflies over radio
  std::string a = address;
  if (address.empty()) {
    a = "E7E7E7E7E7";
  }

  std::vector<std::future<std::string>> futures;
  for (auto datarate : {"250K", "1M", "2M"})
  {
    for (int channel = 0; channel < 125; ++channel) {
      std::string uri = "radio://*/" + std::to_string(channel) + "/" + datarate + "/" + a;

      futures.emplace_back(std::async(std::launch::async,
      [uri]() {
        Connection con(uri);
        bool success;
        while (true)
        {
          if (con.statistics_.sent_count >= 1)
          {
            success = con.statistics_.ack_count >= 1;
            break;
          }
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        };
        if (success) {
          return uri;
        }
        return std::string();
      }));
    }
  }

  for (auto& future : futures) {
    auto uri = future.get();
    if (!uri.empty())
    {
      result.push_back(uri);
    }
  }

  return result;
}

void Connection::send(const Packet& p)
{
  const std::lock_guard<std::mutex> lock(queue_send_mutex_);
  p.seq_ = statistics_.enqueued_count;
  queue_send_.push(p);
  ++statistics_.enqueued_count;
}

Packet Connection::recv(bool blocking)
{
  if (blocking) {
    std::unique_lock<std::mutex> lk(queue_recv_mutex_);
    queue_recv_cv_.wait(lk, [this] { return !queue_recv_.empty(); });
    auto result = queue_recv_.top();
    queue_recv_.pop();
    return result;
  } else {
    const std::lock_guard<std::mutex> lock(queue_recv_mutex_);

    Packet result;
    if (queue_recv_.empty())
    {
      return result;
    } else {
      result = queue_recv_.top();
      queue_recv_.pop();
    }
    return result;
  }
}

std::ostream& operator<<(std::ostream& out, const Connection& p)
{
  out <<"Connection(" << p.uri_;
  out << ")";
  return out;
}