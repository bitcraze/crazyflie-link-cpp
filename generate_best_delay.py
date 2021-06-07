import subprocess
import time
from usb.core import find as finddev

outputs = {}
currDelay = 10
while currDelay < 100:
    dev=finddev(idVendor=0x0483,idProduct=0x5740)
    dev.reset()
    print("currDelay [microsec]:", currDelay)
    try:
        outputs[currDelay] = float(subprocess.check_output(["./build/example_logViaAppChannel", str(currDelay)]).decode()[:-1])
    except KeyboardInterrupt:
        currDelay -= 10
        if input("do you wish to exit? (Y/n)").upper() == "Y":
            exit(0)

    print("rate [bytes/sec]:", outputs[currDelay])

    currDelay +=10
    input("to continue press enter")

print(outputs)