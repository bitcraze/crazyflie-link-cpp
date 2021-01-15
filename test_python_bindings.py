from build import nativelink

if __name__ == "__main__":

  # scan for all available Crazyflies
  cfs = nativelink.Connection.scan("*")
  print(cfs)

  # connect to a CF
  con = nativelink.Connection("test")
  print(con)

  # print connection statistics
  print(con.statistics)

  # send a packet
  p = nativelink.Packet()
  con.send(p)

  # receive a packet
  p = con.recv(blocking=True)
  print(p)
