import sys
import serial
import time

speed = sys.argv[1]

ser1 = serial.Serial("/dev/ttyPCH1",38400)
ser2 = serial.Serial("/dev/ttyPCH2",38400)

def serialSend1(send):
    for i in send:
            ser1.write(chr(i))

def serialSend2(send):
    for i in send:
            ser2.write(chr(i))

def driveM1(speed):
    address = 0x81
    command = 6
    dataBytes = int(speed,0)
    checksum = (address+command+dataBytes)&0x7F
    serialSend1([address,command,dataBytes,checksum])

def driveM2(speed):
    address = 0x80
    command = 6
    dataBytes = int(speed,0)
    checksum = (address+command+dataBytes)&0x7F
    serialSend2([address,command,dataBytes,checksum])

def driveM3(speed):
    address = 0x81
    command = 7
    dataBytes = int(speed,0)
    checksum = (address+command+dataBytes)&0x7F
    serialSend1([address,command,dataBytes,checksum])

driveM1(speed)
driveM2(speed)
driveM3(speed)
