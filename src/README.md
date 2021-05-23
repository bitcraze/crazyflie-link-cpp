# Crazyflie C++ Link - "src" directory

contains all files that are connected to the library

## Files Explained

`Connection.cpp` - created by Bitcraze, contains the implementation of the `Connection` class which is 
reponsible for sending and recieving packets from the crazyflie through crtp.

`ConnectionImpl.h` - created by Bitcraze...

`ConnectionWrapper.cpp` - created by VulcaN, implements the class `ConnectionWrapper` which adds additional functionality
to the `Connection` class like removing the need to use a `Packet` object together with the `Connection` class to send or recieve packets.

`CrazyflieUSB.cpp` - created by Bitcraze, implements all methods in class `CrazyflieUSB`

`CrazyflieUSB.h` - created by Bitcraze, declares the class `CrazyflieUSB` which creates a virtual instance of a 
crazyflie connected via USB.

`CrazyflieUSBThread.cpp` - created by Bitcraze, implements all methods in class `CrazyflieUSBThread`

`CrazyflieUSBThread.h` - created by Bitcraze, declares the class `CrazyflieUSBThread` which contains a thread used in `CrazyflieUSB` class to
