#!/usr/bin/env python3
import cflinkcpp

if __name__ == "__main__":

  # connect to a CF
  con = cflinkcpp.Connection("radio://0/80/2M/E7E7E7E7E7")

  while True:
    p = con.recv(timeout=100)
    if p.valid and p.port == 0 and p.channel == 0:
      print(p.payload.decode('utf-8'), end='')
