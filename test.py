# Sample code for raspberry pi 3
# run python3 -i test.py

import smbus

ADDR = 0x10
READ = 0x22
CLR = 0x33

bus = smbus.SMBus(1)

data = [0,0,0,0]

def read():
  bus.write_byte(ADDR, READ)
  for i in range(0,4):
    data[i] = bus.read_byte(ADDR)
  return data
  
def clear():
  bus.write_byte(ADDR, CLR)
