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

* add test examples to measure bandwidth/latency/packet droprate
  * in C++
  * in Python
* add broadcast support
  * support send2PacketsNoAck 
* add condition variables to lower CPU usage
  * in radio loop
* add namespace 
* improved stats
  * locks?

* rename to cfnativelink
* integrate in Crazyflie_cpp and crazyflie_tools
* code linter
* Add CI for repo

## To Discuss

* bootloader commands:
  * Current solution [feels hacky since it requires specialized URI] -OR- 
  * Add a wait_until_sent_queue empty call instead? [cleanest, but requires interface change] -OR-
  * NRF firmware change to send more than just Ack (e.g., include what was acked, to avoid result being filtered)
* multi-radio broadcast?