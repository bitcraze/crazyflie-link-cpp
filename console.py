from build import nativelink

if __name__ == "__main__":

  # connect to a CF
  con = nativelink.Connection("radio://0/80/2M/E7E7E7E7E7")

  while True:
    p = con.recv(timeout=100)
    if p.valid:
      msg = ''.join(chr(v) for v in p)
      print(msg, end='')