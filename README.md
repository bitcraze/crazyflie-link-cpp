# Crazyflie C++ Link

## Dependencies

```
sudo apt install -y libusb-1.0-0-dev
```

This relies on pybind11, which is a submodule, so don't forget to

```
git submodule init 
git submodule update
```

## Build C++ lib and examples

```
mkdir build
cd build
cmake ..
make
```

The library can be tested using the included example applications, e.g.,:

```
./example_scan
```

## Build and install development version of the python binding

Building the python binding requires python>=3.6, cmake and Ninja. Ninja and CMake can be installed with pip on Windows and Mac (```pip install cmake ninja```), or using apt-get on Ubuntu/debian (```apt-get install cmake ninja-build```).

The following command builds the binding and register it in development mode (ie. as a link) in python.

```
python3 setup.py develop --user
```

The library can be tested using the included example scripts, e.g.,:

```
python3 examples/scan.py
```

## Use as a submodule for c++ project (Linux only)

If you want to use this code as a submodule in your project, you can take the following steps:

1. `sudo apt install -y libusb-1.0-0-dev` # if you haven't already
2. `cd your/project/path`
3. `git submodule add https://github.com/bitcraze/crazyflie-link-cpp crazyflie-link-cpp` 
4. `cd crazyflie-link-cpp`
5. `git submodule init -- libusb/`
6. `git submodule update -- libusb/`
7. `cd ..`
8. copy the content of the `submodule_cmake_template.txt` file to the root folder of your project and into a file named `CMakeLists.txt`
9. follow the comments written in the cmake lists file template to create a cmake that suits you


To compile your project:
1. `mkdir build && cd build`
2. `cmake ..`
3.  `cmake --build .` or just `make`

you can then run the file you wish with `./<executable_file_name>`


## To Do

* add new test that uses echoDelay
* Windows crash

* add broadcast support
  * support send2PacketsNoAck 
* integrate in Crazyflie_cpp and crazyflie_tools
* Hot Plug support (Linux/Mac only)
