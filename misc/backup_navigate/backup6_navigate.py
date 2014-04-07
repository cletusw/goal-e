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
    return int(speed)

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

def readCurrentRpsM1():
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
    return rotations_per_second

def readCurrentRpsM2():
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
    return rotations_per_second

def readCurrentRpsM3():
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
    return rotations_per_second

def rpsComponentM(max_rps,xm,ym,x,y):
    if (x==0 and y==0):
        rpsM = 0
    else:
        x = x/math.sqrt(x*x+y*y)
        y = y/math.sqrt(x*x+y*y)
        rpsM = max_rps*(xm*x+ym*y)/math.sqrt(xm*xm+ym*ym)
    return rpsM

def rpsComponentM1(max_rps,x,y):
    x1 = -math.sqrt(2)/2
    y1 = -math.sqrt(2)/2
    rpsM1 = rpsComponentM(max_rps,x1,y1,x,y)
    return rpsM1

def rpsComponentM2(max_rps,x,y):
    x2 = 1
    y2 = 0
    rpsM2 = rpsComponentM(max_rps,x2,y2,x,y)
    return rpsM2

def rpsComponentM3(max_rps,x,y):
    x3 = -math.sqrt(2)/2
    y3 = math.sqrt(2)/2
    rpsM3 = rpsComponentM(max_rps,x3,y3,x,y)
    return rpsM3

def incrementLogarithmically(rpsCurrent,rpsDesired):
    tune = 0.2
    if (rpsDesired>rpsCurrent):
        inc = math.exp(tune*(rpsDesired-rpsCurrent))
    elif (rpsDesired<rpsCurrent):
        inc = -math.exp(tune*(rpsCurrent-rpsDesired))
    return inc

def goToPoint(max_rps,x,y):
    distance = int(math.sqrt(x*x+y*y))
    rpsM1 = rpsComponentM1(max_rps,x,y)
    rpsM2 = rpsComponentM2(max_rps,x,y)
    rpsM3 = rpsComponentM3(max_rps,x,y)
    speedM1 = 64
    speedM2 = 64
    speedM3 = 64
    for i in range(0,distance):
        driveM1(speedM1)
        driveM2(speedM2)
        driveM3(speedM3)
        speedM1 = speedM1 + (rpsM1 - readCurrentRpsM1())
        speedM2 = speedM2 + (rpsM2 - readCurrentRpsM2())
        speedM3 = speedM3 + (rpsM3 - readCurrentRpsM3())
        print str(readCurrentRpsM1()) + "\t\t" + str(readCurrentRpsM2()) + "\t\t" + str(readCurrentRpsM3())
    stop()

################# Implementation #####################

max_rps = 3
goToPoint(max_rps,x,y)
