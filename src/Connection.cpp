#include <regex>
#include <iostream>
#include <future>

#include "crazyflieLinkCpp/Connection.h"
#include "ConnectionImpl.h"

#include "USBManager.h"
#include "Crazyradio.h"

#include <libusb.h>

namespace bitcraze {
namespace crazyflieLinkCpp {

Connection::Connection(const std::string &uri)
    : impl_(std::make_shared<ConnectionImpl>())
{
  // Examples:
  // "usb://0" -> connect over USB
  // "radio://0/80/2M/E7E7E7E7E7" -> connect over radio
  // "radio://*/80/2M/E7E7E7E7E7" -> auto-pick radio
  // "radio://*/80/2M/*" -> broadcast/P2P sniffing on channel 80
  // "radiobroadcast://0/80/2M -> broadcast to all crazyflies on channel 80

  const std::regex uri_regex("(usb:\\/\\/(\\d+)|radio:\\/\\/(\\d+|\\*)\\/(\\d+)\\/(250K|1M|2M)\\/([a-fA-F0-9]+|\\*)(\\?[\\w=&]+)?)|(radiobroadcast:\\/\\/(\\d+|\\*)\\/(\\d+)\\/(250K|1M|2M))");
  std::smatch match;
  if (!std::regex_match(uri, match, uri_regex)) {
    std::stringstream sstr;
    sstr << "Invalid uri (" << uri << ")!";
    throw std::runtime_error(sstr.str());
  }

  impl_->uri_ = uri;

  // for (size_t i = 0; i < match.size(); ++i) {
  //   std::cout << i << " " << match[i].str() << std::endl;
  // }
  // std::cout << match.size() << std::endl;

  if (match[2].matched) {
    // usb://
    impl_->devid_ = std::stoi(match[2].str());

    if (impl_->devid_ < 0 || impl_->devid_ >= (int)USBManager::get().numCrazyfliesOverUSB()) {
      std::stringstream sstr;
      sstr << "No Crazyflie with id=" << impl_->devid_ << " found. ";
      sstr << "There are " << USBManager::get().numCrazyfliesOverUSB() << " Crazyflies connected.";
      throw std::runtime_error(sstr.str());
    }
    impl_->isRadio_ = false;
  } else {
    // radio
    uint8_t devIdMatchIndex = 3;
    uint8_t channelMatchIndex = 4;
    uint8_t datarateMatchIndex = 5;

    // broadcast
    if (match[8].matched) {
        impl_->address_ = BROADCAST_ADDRESS;
        impl_->useSafelink_ = false;
        impl_->useAutoPing_ = false;
        impl_->useAckFilter_ = false;
        impl_->broadcast_ = true;
        devIdMatchIndex = 9;
        channelMatchIndex = 10;
        datarateMatchIndex = 11;
    }
    else {
        // Address is represented by 40 bytes => 10 hex chars max
        if (match[6].str().length() > 10) {
            std::stringstream sstr;
            sstr << "Invalid uri (" << uri << ")!";
            throw std::runtime_error(sstr.str());
        }
        impl_->address_ = std::stoull(match[6].str(), nullptr, 16);

        // parse flags
        impl_->useSafelink_ = true;
        impl_->useAutoPing_ = true;
        impl_->useAckFilter_ = true;
        impl_->broadcast_ = false;

        if (match[7].length() > 0) {
            std::stringstream sstr(match[7].str().substr(1));
            std::string keyvalue;
            const std::regex params_regex("(safelink|autoping|ackfilter|broadcast)=([0|1])");
            while (getline(sstr, keyvalue, '&')) {
                std::smatch match;
                if (!std::regex_match(keyvalue, match, params_regex)) {
                    std::stringstream sstr;
                    sstr << "Invalid uri (" << uri << ")! ";
                    sstr << "Unknown " << keyvalue << ".";
                    throw std::runtime_error(sstr.str());
                }

                if (match[1].str() == "safelink" && match[2].str() == "0") {
                    impl_->useSafelink_ = false;
                }
                if (match[1].str() == "autoping" && match[2].str() == "0") {
                    impl_->useAutoPing_ = false;
                }
                if (match[1].str() == "ackfilter" && match[2].str() == "0") {
                    impl_->useAckFilter_ = false;
                }
                if (match[1].str() == "broadcast" && match[2].str() == "1") {
                    impl_->broadcast_ = true;
                }
            }
        }
    }

    if (match[devIdMatchIndex].str() == "*") {
        impl_->devid_ = -1;
    }
    else {
        impl_->devid_ = std::stoi(match[devIdMatchIndex].str());
    }

    impl_->channel_ = std::stoi(match[channelMatchIndex].str());
    if (match[datarateMatchIndex].str() == "250K") {
        impl_->datarate_ = Crazyradio::Datarate_250KPS;
    }
    else if (match[datarateMatchIndex].str() == "1M") {
        impl_->datarate_ = Crazyradio::Datarate_1MPS;
    }
    else if (match[datarateMatchIndex].str() == "2M") {
        impl_->datarate_ = Crazyradio::Datarate_2MPS;
    }

    impl_->safelinkInitialized_ = false;
    impl_->safelinkUp_ = false;
    impl_->safelinkDown_ = false;

    impl_->isRadio_ = true;
  }

  USBManager::get().addConnection(impl_);
}

Connection::~Connection()
{
  close();
}

void Connection::close()
{
  if (impl_->devid_ >= 0) {
    USBManager::get().removeConnection(impl_);
  }
}

std::vector<std::string> Connection::scan(uint64_t address)
{
  USBManager::get().updateDevices();

  std::vector<std::string> result;

  // Crazyflies over USB
  for (size_t i = 0; i < USBManager::get().numCrazyfliesOverUSB(); ++i) {
    result.push_back("usb://" + std::to_string(i));
  }

  // Crazyflies over radio
  if (USBManager::get().numCrazyradios()) {
    std::stringstream sstr;
    sstr << std::hex << std::uppercase << address;
    std::string a = sstr.str();

    std::vector<std::future<std::string>> futures;
    for (auto datarate : {"250K", "1M", "2M"})
    {
      for (int channel = 0; channel < 125; ++channel) {
        std::string uri = "radio://*/" + std::to_string(channel) + "/" + datarate + "/" + a;

        futures.emplace_back(std::async(std::launch::async,
        [uri]() {
          try {
            Connection con(uri);
            bool success;
            while (true)
            {
              if (con.statistics().sent_count >= 1)
              {
                success = con.statistics().ack_count >= 1;
                break;
              }
              std::this_thread::sleep_for(std::chrono::milliseconds(1));
            };
            if (success) {
              return uri;
            }
          } catch(...) {
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
  }

  return result;
}

std::vector<std::string> Connection::scan_selected(const std::vector<std::string> &uris)
{
  std::vector<std::string> result;
  std::vector<std::future<std::string>> futures;
  for (const auto& uri : uris) {
    futures.emplace_back(std::async(std::launch::async,
      [uri]() {
        try
        {
          Connection con(uri + "?safelink=0&autoping=0&ackfilter=0");
          const uint8_t check[] = {0xFF, 0xFF, 0xFF};
          Packet p(check, sizeof(check));
          con.send(p);
          do {
            Packet p_recv = con.recv(10);
            if (p_recv)
            {
              return uri;
            }
          } while (con.statistics().sent_count == 0);
        }
        catch (const std::runtime_error& e)
        {
          std::cout << "Error during scan: " << e.what() << std::endl;
        }
        return std::string();
      }));
  }

  for (auto &future : futures)
  {
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
  const std::lock_guard<std::mutex> lock(impl_->queue_send_mutex_);
  if (!impl_->runtime_error_.empty()) {
    throw std::runtime_error(impl_->runtime_error_);
  }

  p.seq_ = impl_->statistics_.enqueued_count;
  impl_->queue_send_.push(p);
  ++impl_->statistics_.enqueued_count;
}

Packet Connection::receive(unsigned int timeout_in_ms)
{
  std::unique_lock<std::mutex> lk(impl_->queue_recv_mutex_);
  if (!impl_->runtime_error_.empty()) {
    throw std::runtime_error(impl_->runtime_error_);
  }
  if (timeout_in_ms == Connection::TimeoutNone) {
    /* nothing needs to be done */
  } else if (timeout_in_ms == Connection::TimeoutBlock) {
    impl_->queue_recv_cv_.wait(lk, [this] { return !impl_->queue_recv_.empty(); });
  } else {
    std::chrono::milliseconds duration(timeout_in_ms);
    impl_->queue_recv_cv_.wait_for(lk, duration, [this] { return !impl_->queue_recv_.empty(); });
  }

  Packet result;
  if (impl_->queue_recv_.empty()) {
    return result;
  } else {
    result = impl_->queue_recv_.top();
    impl_->queue_recv_.pop();
  }
  return result;
}

Packet Connection::recv(unsigned int timeout_in_ms)
{
  unsigned int timeout_in_ms_new = timeout_in_ms == 0 ? Connection::TimeoutBlock : timeout_in_ms;
  return receive(timeout_in_ms_new);
}

std::ostream& operator<<(std::ostream& out, const Connection& p)
{
  out <<"Connection(" << p.impl_->uri_;
  out << ")";
  return out;
}

const std::string& Connection::uri() const
{
  return impl_->uri_;
}

const Connection::Statistics Connection::statistics() const
{
  if (!impl_->runtime_error_.empty()) {
    throw std::runtime_error(impl_->runtime_error_);
  }
  return impl_->statistics_;
}

const Connection::Statistics Connection::statisticsDelta()
{
  if (!impl_->runtime_error_.empty()) {
    throw std::runtime_error(impl_->runtime_error_);
  }
  auto current = impl_->statistics_;
  auto result = current - impl_->statistics_previous_;
  impl_->statistics_previous_ = current;
  return result;
}

} // namespace crazyflieLinkCpp
} // namespace bitcraze