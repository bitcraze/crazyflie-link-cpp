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
find_library(USB_LIB usb-1.0)

include_directories(
  include
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
)

# target_include_directories(libCrazyflieCppLink
#   PUBLIC
#     $<INSTALL_INTERFACE:include/crazyflie_cpp>
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/crazyflie_cpp>
# )

target_link_libraries(crazyflieCppLink
  PRIVATE
    ${USB_LIB}
    pthread
)
set_property(TARGET crazyflieCppLink PROPERTY POSITION_INDEPENDENT_CODE ON)

# C++ test application

# test_console
add_executable(test_console
  test/console.cpp
)
target_link_libraries(test_console
  crazyflieCppLink
)

# test_scan
add_executable(test_scan
  test/scan.cpp
)
target_link_libraries(test_scan
  crazyflieCppLink
)

# test_benchmark
add_executable(test_benchmark
  test/benchmark.cpp
)
target_link_libraries(test_benchmark
  crazyflieCppLink
)

# scan
add_executable(test
  test/main.cpp
)
target_link_libraries(test
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