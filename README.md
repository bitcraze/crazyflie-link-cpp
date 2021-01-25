# native-link


## Dependencies

```
sudo apt install -y libusb-1.0-0-dev
```

This relies on pybind11, which is a submodule, so don't forget to

```
git submodule init 
git submodule update
```

## Build

```
mkdir build
cd build
cmake ..
make
```

## Run

### Testing Python Bindings

This uses the bindings in `build` and has some examples on how to use the bindings.

```
python3 test_python_binding.py
```

## To Do

* add condition variables to lower CPU usage
  * in radio loop
* add scanning based on connection instances and async (single radio)
* add radio scheduler (* notation)
* switch scanning to radio scheduler to use multiple radios
* add test examples to measure bandwidth/latency/packet droprate
  * in C++
  * in Python
* add broadcast support
* add CrazyflieUSB support
* better separation public/private interface for lib
* integrate in Crazyflie_cpp and crazyflie_tools
* integrate in python client
* test with 'faulty' radio
* code linter
* Add CI for repo

## To Discuss

* multi-radio broadcast?
