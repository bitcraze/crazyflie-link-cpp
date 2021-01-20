#include "Connection.h"

#include "USBManager.h"
#include "Crazyradio.h"

#include <libusb-1.0/libusb.h>


Connection::Connection(const std::string& uri)
  : uri_(uri)
{

}

Connection::~Connection()
{

}

std::vector<std::string> Connection::scan(const std::string& /*address*/)
{
  std::vector<std::string> result;

  // Crazyflies over USB
  for (size_t i = 0; i < USBManager::get().numCrazyfliesOverUSB(); ++i) {
    result.push_back("usb://" + std::to_string(i));
  }

  // Crazyflies over radio
  // TODO: use connection istances to support multiple radios

  return result;
}

void Connection::send(const Packet& p)
{
  queue_send_.push(p);
}

Packet Connection::recv(bool /*blocking*/)
{
  return Packet();
}

std::ostream& operator<<(std::ostream& out, const Connection& p)
{
  out <<"Connection(" << p.uri_;
  out << ")";
  return out;
}