#!/usr/bin/env python3
import cflinkcpp
import time
import struct

class PacketUtils:

  @staticmethod
  def takeoff(height, yaw, time):
    nativePk = cflinkcpp.Packet()
    nativePk.port = 8  # Port 8 = HL Commander
    nativePk.payload = struct.pack('<BBff?f',
                                    7,        # Command (7 = takeoff)
                                    0,        # Group Mask (0 = all groups)
                                    height,
                                    yaw,
                                    False,    # Use current yaw
                                    time)     # Duration (seconds)
    return nativePk


  @staticmethod
  def land(height, yaw, time):
    nativePk = cflinkcpp.Packet()
    nativePk.port = 8  # Port 8 = HL Commander
    nativePk.payload = struct.pack('<BBff?f',
                                    8,        # Command (8 = land)
                                    0,        # Group Mask (0 = all groups)
                                    height,
                                    yaw,
                                    False,    # Use current yaw
                                    time)     # Duration (seconds)
    return nativePk

  @staticmethod
  def stop():
      nativePk = cflinkcpp.Packet()
      nativePk.port = 8  # Port 8 = HL Commander
      nativePk.payload = struct.pack('<BB',
                                     3,  # Command (3 = stop)
                                     0)  # Group Mask (0 = all groups)
      return nativePk


if __name__ == "__main__":
  """
   Creates a connection that will broadcast to all
   crazyflies on channel 80. Note that this requires
   updating the Crazyradio firmware from its factory version
   """
  broadcastConnection = cflinkcpp.Connection("radiobroadcast://0/80/2M")

  # Requires high level commander enabled (param commander.enHighLevel == 1)
  print("Taking off...")
  broadcastConnection.send(PacketUtils.takeoff(0.5, 0, 3))
  time.sleep(3.25)

  print("Landing...")
  broadcastConnection.send(PacketUtils.takeoff(0.5, 0, 3))
  time.sleep(3.25)

  print("Stopping...")
  broadcastConnection.send(PacketUtils.stop())
  time.sleep(0.5)

  broadcastConnection.close()
  print("Done.")
