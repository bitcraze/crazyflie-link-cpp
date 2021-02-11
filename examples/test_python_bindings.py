#!/usr/bin/env python3
import cfcpplink

if __name__ == "__main__":

  # scan for all available Crazyflies
  cfs = cfcpplink.Connection.scan()
  print(cfs)

  # connect to a CF
  # con = cfcpplink.Connection("usb://0")
  con = cfcpplink.Connection("radio://0/80/2M/E7E7E7E7E7")

  while True:
    p = con.recv(timeout=100)
    print(p)

  # print(con)

  # # print connection statistics
  # print(con.statistics)

  # # send a packet
  # p = cfcpplink.Packet()
  # p.port = 15     # CRTP_PORT_LINK
  # p.channel = 0   # ECHO
  # p.size = 1
  # p.data[0] = 5
  # print(p)
  # con.send(p)

  # # receive a packet
  # p = con.recv(blocking=True)
  # print(p)
