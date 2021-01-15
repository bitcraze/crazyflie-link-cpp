#include "Connection.h"

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

  return result;
}

void Connection::send(const Packet& /*p*/)
{

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