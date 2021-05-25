# Crazyflie C++ Link - "src" directory

contains all files that are connected to the library

## Files Explained

### `ConnectionWrapper.cpp`

#### Class `ConnectionWrapper` 
Adds additional functionality to the `Connection` class like removing the need to use a `Packet` object together with the `Connection` class to send or recieve packets.

 **`recvFilteredData`** - returns the first Packet recieved with the specified port and channel. If no port and channel specified it uses the same port and channel used by the ConnectionWrapper itself.
 
 



