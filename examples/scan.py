#!/usr/bin/env python3
import cfcpplink

if __name__ == "__main__":

  # scan for all available Crazyflies
  cfs = cfcpplink.Connection.scan()
  print(cfs)
