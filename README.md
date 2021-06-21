# Crazyflie C++ Lib 

A library which you could include into your c++ project and run the functions in here to code yourself a program for the crazyflie. A c++ version of [crazyflie-lib-python](https://github.com/bitcraze/crazyflie-lib-python)

## Development

### Repo Structure

* `src` - source files for crazyflie lib classes and functions

* `examples` - example files which you can run

* `include` - contains all header files for crazyflie lib

### For Visual Studio Editting Best Experience

1. Install c/c++ add-on
2. Install CMakeLists add-on
3. Open Directory in Vscode
4. Allow intelisense for c/c++
5. Allow intelisense for CMakeLists (let the compiler guess which g++ version to use) 



## How to run

### Dependencies

```
sudo apt install -y libusb-1.0-0-dev
```

### Build C++ lib and examples

```
mkdir build
cd build
cmake ..
make
```

The library can be tested from the build directory using the example applications, e.g.,:

```
./example_scan
```





