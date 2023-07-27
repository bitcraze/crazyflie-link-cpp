cmake_minimum_required (VERSION 3.5)
project(crazyflie-link-cpp)

# define some options
option(BUILD_PYTHON_BINDINGS "Generate Python Bindings" ON)
option(BUILD_CPP_EXAMPLES "Generate C++ Examples" ON)

# Enable C++14 and warnings
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compile static libraries with hidden visibility
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if (MSVC)
  # Avoid min/max macros in Windows.h (pulled in from libusb)
  add_definitions(-DNOMINMAX)
  # warning level 4
  # Ideally we would also treat warnings as errors (/WX), but there are still some warnings to fix
  add_compile_options(/W4)
else()
  # lots of warnings and all warnings as errors
  add_compile_options(-Wall -Wextra -Werror)
endif()


# dependencies
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tools/build)

if (WIN32)
  # build libusb
  include_directories(
    libusb/msvc
  )
  add_library(libusb
    libusb/libusb/core.c
    libusb/libusb/descriptor.c
    libusb/libusb/os/events_windows.c
    libusb/libusb/hotplug.c
    libusb/libusb/io.c
    libusb/libusb/strerror.c
    libusb/libusb/sync.c
    libusb/libusb/os/threads_windows.c
    libusb/libusb/os/windows_common.c
    libusb/libusb/os/windows_usbdk.c
    libusb/libusb/os/windows_winusb.c
  )
  set(LIBUSB_INCLUDE_DIR libusb/libusb)
  set(LIBUSB_LIBRARY libusb)
else()
  find_package(libusb REQUIRED)
endif()

include_directories(
  include
  ${LIBUSB_INCLUDE_DIR}
)

# C++ library

add_library(crazyflieLinkCpp
  src/USBManager.cpp
  src/Connection.cpp
  src/USBDevice.cpp
  src/Crazyradio.cpp
  src/CrazyradioThread.cpp
  src/CrazyflieUSB.cpp
  src/CrazyflieUSBThread.cpp
  src/Version.cpp
)

# target_include_directories(libCrazyflieLinkCpp
#   PUBLIC
#     $<INSTALL_INTERFACE:include/crazyflie_cpp>
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/crazyflie_cpp>
# )

# Apple macOS libraries
if (APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "-lobjc -framework IOKit -framework CoreFoundation -framework Security")
  set(CMAKE_MODULE_LINKER_FLAGS "-lobjc -framework IOKit -framework CoreFoundation -framework Security")
endif()

# Link pthread on Linux and Mac only
if (NOT MSVC)
target_link_libraries(crazyflieLinkCpp
  PRIVATE
    pthread
)
endif()

target_link_libraries(crazyflieLinkCpp
  PRIVATE
    ${LIBUSB_LIBRARY}
)
set_property(TARGET crazyflieLinkCpp PROPERTY POSITION_INDEPENDENT_CODE ON)

# C++ example application
if (BUILD_CPP_EXAMPLES)
  # example_console
  add_executable(example_console
    examples/console.cpp
  )
  target_link_libraries(example_console
    crazyflieLinkCpp
  )

  # example_scan
  add_executable(example_scan
    examples/scan.cpp
  )
  target_link_libraries(example_scan
    crazyflieLinkCpp
  )

  # example_benchmark
  add_executable(example_benchmark
    examples/benchmark.cpp
  )
  target_link_libraries(example_benchmark
    crazyflieLinkCpp
  )

  # example_broadcast
  add_executable(example_broadcast
    examples/broadcast.cpp
  )
  target_link_libraries(example_broadcast
    crazyflieLinkCpp
  )

  # example_broadcast_and_console
  add_executable(example_broadcast_and_console
    examples/broadcast_and_console.cpp
  )
  target_link_libraries(example_broadcast_and_console
    crazyflieLinkCpp
  )
endif()

if (BUILD_PYTHON_BINDINGS)
  # Python bindings
  add_subdirectory(pybind11 EXCLUDE_FROM_ALL)

  pybind11_add_module(cflinkcpp
    src/python_bindings.cpp
  )

  target_link_libraries(cflinkcpp
    PRIVATE
      crazyflieLinkCpp
  )
endif()