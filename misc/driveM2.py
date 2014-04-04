import sys
import serial

speed = sys.argv[1]

ser2 = serial.Serial("/dev/ttyPCH2",38400)

def serialSend2(send):
    for i in send:
            ser2.write(chr(i))

def driveM2(speed):
    address = 0x80
    command = 6
    dataBytes = int(speed,0)
    checksum = (address+command+dataBytes)&0x7F
    serialSend2([address,command,dataBytes,checksum])

driveM2(speed)

