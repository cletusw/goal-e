import sys
import serial
import time

speed = sys.argv[1]

port1 = "/dev/ttyPCH1"
port2 = "/dev/ttyPCH2"
baudRate = 38400

ser1 = serial.Serial(port1,baudRate)
ser2 = serial.Serial(port2,baudRate)

def serialSend1(send):
    for i in send:
            ser1.write(chr(i))

def serialSend2(send):
    for i in send:
            ser2.write(chr(i))

def serialRead1(read):
    serialSend1(read)
    data=[]
    for i in range(6):
        data.append(ser1.read())
    return data

def serialRead2(read):
    serialSend2(read)
    data=[]
    for i in range(6):
        data.append(ser2.read())
    return data

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

def readCurrentSpeedM1():
    address = 0x81
    command = 30
    # Read the current speed from the motor.
    data = serialRead1([address,command])
    # Read the first four bytes (in hex) as the speed.
    speed = (data[0].encode("hex")) + (data[1].encode("hex")) + (data[2].encode("hex")) + (data[3].encode("hex"))
	# Convert the speed to an integer.
    speed = int(speed, 16)
	# If the fifth byte indicates that the speed is negative, take the 2's complement.
    if ((ord(data[4]) == 1) and (speed != 0)):
		speed = ~(0xffffffff - speed) + 1
    # Calculate the speed in rotations per second (negating to account for wiring).
    rotations_per_second = -float(speed) * 125 / 8192 
    print 'M1 Speed:\n' + str(rotations_per_second) 
    return rotations_per_second

def readCurrentSpeedM2():
    address = 0x80
    command = 30
    # Read the speed information from the motor.
    data = serialRead2([address,command])
    # Read the first four bytes (in hex) as the speed.
    speed = (data[0].encode("hex")) + (data[1].encode("hex")) + (data[2].encode("hex")) + (data[3].encode("hex"))
	# Convert the speed to an integer.
    speed = int(speed, 16)
	# If the fifth byte indicates that the value is negative, take the 2's complement.
    if ((ord(data[4]) == 1) and (speed != 0)):
		speed = ~(0xffffffff - speed) + 1
    # Calculate the speed in rotations per second (negating to account for wiring).
    rotations_per_second = -float(speed) * 125 / 8192 
    print 'M2 Speed:\n' + str(rotations_per_second) 
    return rotations_per_second

def readCurrentSpeedM3():
    address = 0x81
    command = 31
    # Read the current speed from the motor.
    data = serialRead1([address,command])
    # Read the first four bytes (in hex) as the speed.
    speed = (data[0].encode("hex")) + (data[1].encode("hex")) + (data[2].encode("hex")) + (data[3].encode("hex"))
	# Convert the speed to an integer.
    speed = int(speed, 16)
	# If the fifth byte indicates that the speed is negative, take the 2's complement.
    if ((ord(data[4]) == 1) and (speed != 0)):
		speed = ~(0xffffffff - speed) + 1
    # Calculate the speed in rotations per second (negating to account for wiring).
    rotations_per_second = -float(speed) * 125 / 8192 
    print 'M3 Speed:\n' + str(rotations_per_second) 
    return rotations_per_second

driveM1(speed)
driveM2(speed)
driveM3(speed)
time.sleep(1)
readCurrentSpeedM1()
readCurrentSpeedM2()
readCurrentSpeedM3()

