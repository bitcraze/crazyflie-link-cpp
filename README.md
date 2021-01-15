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