# Crazyflie C++ Link - "src" directory

contains all files that are connected to the library

## Files Explained

**`isBigEndian`** - returns true if the current cpu is big edian and false if it is small edian.

### `ConnectionWrapper.cpp`

#### Class `ConnectionWrapper` 
Adds additional functionality to the `Connection` class like removing the need to use a `Packet` object together with the `Connection` class to send or recieve packets.

**`ConnectionWrapper`** - Takes a connection as an arguement, the class acts as a 'wrapper' to that connection given to the constructor, which it keeps an reference of.

**`recvFilteredData`** - returns the first Packet recieved with the specified port and channel. If no port and channel specified it uses the same port and channel used by the ConnectionWrapper itself.

**`sendData`** - takes 2 or 4 arguements. The pointer of the data and the second argument is the size of the data. It combins the first and second data together (if porvided) and sends it all to the crazyflie.





 
 



