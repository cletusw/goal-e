import sys
import serial
import time
import math

x = float(sys.argv[1])
y = float(sys.argv[2])
#theta = sys.argv[3]

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

def threshold(speed):
    if (speed < 0):
        speed = 0
    elif (speed > 127):
        speed = 127
    return speed

def driveM1(speed):
    address = 0x81
    command = 6
    dataBytes = threshold(speed)
    checksum = (address+command+dataBytes)&0x7F
    serialSend1([address,command,dataBytes,checksum])

def driveM2(speed):
    address = 0x80
    command = 6
    dataBytes = threshold(speed)
    checksum = (address+command+dataBytes)&0x7F
    serialSend2([address,command,dataBytes,checksum])

def driveM3(speed):
    address = 0x81
    command = 7
    dataBytes = threshold(speed)
    checksum = (address+command+dataBytes)&0x7F
    serialSend1([address,command,dataBytes,checksum])

def driveAll(speed):
    driveM1(speed)
    driveM2(speed)
    driveM3(speed)

def stop():
    driveAll(64)

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

def speedComponentM(speed,xm,ym,x,y):
    speed = speed - 64
    x = x/math.sqrt(x*x+y*y)
    y = y/math.sqrt(x*x+y*y)
    speedM = speed*(xm*x+ym*y)/math.sqrt(xm*xm+ym*ym)
    return int(speedM+64)

def speedComponentM1(speed,x,y):
    x1 = -math.sqrt(2)/2
    y1 = -math.sqrt(2)/2
    speedM1 = speedComponentM(speed,x1,y1,x,y)
    return speedM1

def speedComponentM2(speed,x,y):
    x2 = 1
    y2 = 0
    speedM2 = speedComponentM(speed,x2,y2,x,y)
    return speedM2

def speedComponentM3(speed,x,y):
    x3 = -math.sqrt(2)/2
    y3 = math.sqrt(2)/2
    speedM3 = speedComponentM(speed,x3,y3,x,y)
    return speedM3

def goToPoint(speed,x,y):
    distance = 50;
    for i in range(0,distance):
        driveM1(speedComponentM1(speed,x,y))
        driveM2(speedComponentM2(speed,x,y))
        driveM3(speedComponentM3(speed,x,y))

################# Implementation #####################

speed = 70
goToPoint(speed,x,y)

time.sleep(0.10)
readCurrentSpeedM1()
readCurrentSpeedM2()
readCurrentSpeedM3()
time.sleep(3)
stop()
