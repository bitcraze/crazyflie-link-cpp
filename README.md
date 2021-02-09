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

## Build C++ examples

```
mkdir build
cd build
cmake ..
make
```

## Build and install development version of the python binding

The followind command builds the binding and register it in development mode (ie. as a link) in python.

```
python3 setup.py develop --user
```

## Run

### Testing Python Bindings

This uses the bindings installed and has some examples on how to use the bindings.

```
python3 test/test_python_binding.py
```

## To Do

* improve behavior after re-plugging in client => track by serial number
* add namespace 
* improved stats
  * locks?

* add license: MIT
* rename to cfcpplink
* code linter
* Add CI for repo

* add broadcast support
  * support send2PacketsNoAck 
* integrate in Crazyflie_cpp and crazyflie_tools

## To Discuss
