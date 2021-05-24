# Crazyflie C++ Link

## Overview

```
This project allow us to get parameter value from a specific parameter in a specific group.

```

## Dependencies

```
sudo apt install -y libusb-1.0-0-dev
```

This relies on pybind11, which is a submodule, so don't forget to

```
git submodule init 
git submodule update
```

## Build C++ lib 

```
mkdir build
cd build
cmake ..
make
```

now run the specific project

```
./example_basicParam
```
