import sys
import serial

speed = sys.argv[1]

ser1 = serial.Serial("/dev/ttyPCH1",38400)

def serialSend1(send):
    for i in send:
            ser1.write(chr(i))

def driveM1(speed):
    address = 0x81
    command = 6
    dataBytes = int(speed,0)
    checksum = (address+command+dataBytes)&0x7F
    serialSend1([address,command,dataBytes,checksum])

driveM1(speed)

