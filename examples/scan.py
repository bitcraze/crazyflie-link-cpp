#!/usr/bin/env python3
import cflinkcpp

if __name__ == "__main__":

  # scan for all available Crazyflies
  cfs = cflinkcpp.Connection.scan()
  print(cfs)
