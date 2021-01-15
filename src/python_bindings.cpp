#include <sstream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "Packet.hpp"
#include "Connection.h"

PYBIND11_MAKE_OPAQUE(std::array<uint8_t, CRTP_MAXSIZE>)

namespace py = pybind11;
using namespace pybind11::literals;

template<class T>
std::string toString(const T& x) 
{
  std::stringstream sstr;
  sstr << x;
  return sstr.str();
}

PYBIND11_MODULE(nativelink, m) {

  // Helper for Packet
  // py::bind_vector<std::array<uint8_t, CRTP_MAXSIZE>>(m, "ByteArray");

  // Packet
  py::class_<Packet>(m, "Packet")
      .def(py::init<>())
      // .def_readwrite("channel", &Packet::channel)
      // .def_readwrite("port", &Packet::port)
      .def_readonly("data", &Packet::data)
      .def_readwrite("size", &Packet::size)
      // .def_property("size", &Packet::size, &Packet::setSize)
      .def("__repr__", &toString<Packet>);

  //
  py::class_<Connection::Statistics>(m, "ConnectionStatistics")
      .def_readonly("sent_count", &Connection::Statistics::sent_count)
      .def("__repr__", &toString<Connection::Statistics>);

  // Connection
  py::class_<Connection>(m, "Connection")
      .def(py::init<
           const std::string &>())
      .def_static("scan", &Connection::scan)
      .def("send", &Connection::send)
      .def("recv", &Connection::recv, py::arg("blocking"))
      .def_property_readonly("uri", &Connection::uri)
      .def_property_readonly("statistics", &Connection::statistics)
      .def("__repr__", &toString<Connection>);
}
