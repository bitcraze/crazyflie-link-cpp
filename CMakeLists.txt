cmake_minimum_required (VERSION 3.5)
project(crazyflie-cpp-link)


# Enable C++14 and warnings
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
  # warning level 4 and all warnings as errors
  add_compile_options(/W4 /WX)
else()
  # lots of warnings and all warnings as errors
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()


# dependencies
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/tools/build)
find_package(libusb REQUIRED)

include_directories(
  include
  ${LIBUSB_INCLUDE_DIR}
)

# C++ library

add_library(crazyflieCppLink
  src/USBManager.cpp
  src/Connection.cpp
  src/USBDevice.cpp
  src/Crazyradio.cpp
  src/CrazyradioThread.cpp
  src/CrazyflieUSB.cpp
  src/CrazyflieUSBThread.cpp
  src/Version.cpp
)

# target_include_directories(libCrazyflieCppLink
#   PUBLIC
#     $<INSTALL_INTERFACE:include/crazyflie_cpp>
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/crazyflie_cpp>
# )

target_link_libraries(crazyflieCppLink
  PRIVATE
    ${LIBUSB_LIBRARY}
    pthread
)
set_property(TARGET crazyflieCppLink PROPERTY POSITION_INDEPENDENT_CODE ON)

# C++ example application

# example_console
add_executable(example_console
  examples/console.cpp
)
target_link_libraries(example_console
  crazyflieCppLink
)

# example_scan
add_executable(example_scan
  examples/scan.cpp
)
target_link_libraries(example_scan
  crazyflieCppLink
)

# example_benchmark
add_executable(example_benchmark
  examples/benchmark.cpp
)
target_link_libraries(example_benchmark
  crazyflieCppLink
)

# Python bindings
add_subdirectory(pybind11)

pybind11_add_module(cfcpplink
  src/python_bindings.cpp
)

target_link_libraries(cfcpplink
  PRIVATE
    crazyflieCppLink
)