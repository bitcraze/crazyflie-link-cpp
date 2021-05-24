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

## For Visual Studio Editting Best Experience

1. Install c/c++ add-on
2. Install CMakeLists add-on
3. Allow intelisense for c/c++
4. Allow intelisense for CMakeLists (let the compiler guess which g++ version to use) 


## Build and install development version of the python binding

**Building the python binding requires python>=3.6**, cmake and Ninja. Ninja and CMake can be installed with pip on Windows and Mac (```pip install cmake ninja```), or using apt-get on Ubuntu/debian (```apt-get install cmake ninja-build```).

The following command builds the binding and register it in development mode (ie. as a link) in python.

```
python3 setup.py develop --user
```

The library can be tested using the included example scripts, e.g.,:

```
python3 examples/scan.py
```
