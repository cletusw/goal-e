# Author: Cameron Taylor
# Last Modified: 7 Apr 2014
# Given an angle and an x,y coordinate, go to point and then turn.

import sys
import serial
import time
import math

class Navigate:
    port1 = "/dev/ttyPCH1"
    port2 = "/dev/ttyPCH2"
    baudRate = 38400
    ser1 = None
    ser2 = None

    def __init__(self):
        self.ser1 = serial.Serial(self.port1, self.baudRate)
        self.ser2 = serial.Serial(self.port2, self.baudRate)

    def serialSend1(self, send):
        for i in send:
            self.ser1.write(chr(i))

    def serialSend2(self, send):
        for i in send:
            self.ser2.write(chr(i))

    def serialRead1(self, read):
        self.serialSend1(read)
        data=[]
        for i in range(6):
            data.append(self.ser1.read())
        return data

    def serialRead2(self, read):
        self.serialSend2(read)
        data=[]
        for i in range(6):
            data.append(self.ser2.read())
        return data

    def threshold(self, speed):
        if (speed < 0):
            speed = 0
        elif (speed > 127):
            speed = 127
        return int(speed)

    def driveM1(self, speed):
        address = 0x81
        command = 6
        dataBytes = self.threshold(speed)
        checksum = (address+command+dataBytes)&0x7F
        self.serialSend1([address,command,dataBytes,checksum])

    def driveM2(self, speed):
        address = 0x80
        command = 6
        dataBytes = self.threshold(speed)
        checksum = (address+command+dataBytes)&0x7F
        self.serialSend2([address,command,dataBytes,checksum])

    def driveM3(self, speed):
        address = 0x81
        command = 7
        dataBytes = self.threshold(speed)
        checksum = (address+command+dataBytes)&0x7F
        self.serialSend1([address,command,dataBytes,checksum])

    def driveAll(self, speed):
        self.driveM1(speed)
        self.driveM2(speed)
        self.driveM3(speed)

    def stop(self):
        self.driveAll(64)

    def readCurrentRpsM1(self):
        address = 0x81
        command = 30
        # Read the current speed from the motor.
        data = self.serialRead1([address,command])
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

    def readCurrentRpsM2(self):
        address = 0x80
        command = 30
        # Read the speed information from the motor.
        data = self.serialRead2([address,command])
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

    def readCurrentRpsM3(self):
        address = 0x81
        command = 31
        # Read the current speed from the motor.
        data = self.serialRead1([address,command])
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

    def rpsComponentM(self, max_rps,xm,ym,x,y):
        if (x==0 and y==0):
            rpsM = 0
        else:
            x = x/math.sqrt(x*x+y*y)
            y = y/math.sqrt(x*x+y*y)
            rpsM = max_rps*(xm*x+ym*y)/math.sqrt(xm*xm+ym*ym)
        return rpsM

    def rpsComponentM1(self, max_rps,x,y):
        x1 = -math.sqrt(2)/2
        y1 = -math.sqrt(2)/2
        rpsM1 = self.rpsComponentM(max_rps,x1,y1,x,y)
        return rpsM1

    def rpsComponentM2(self, max_rps,x,y):
        x2 = 1
        y2 = 0
        rpsM2 = self.rpsComponentM(max_rps,x2,y2,x,y)
        return rpsM2

    def rpsComponentM3(self, max_rps,x,y):
        x3 = -math.sqrt(2)/2
        y3 = math.sqrt(2)/2
        rpsM3 = self.rpsComponentM(max_rps,x3,y3,x,y)
        return rpsM3

    # Rotate between -180 and +180 degrees
    def rotate(self, max_rps,theta):
        print "Rotating " + str(theta) + " degrees."
        tune_angle = 0.3;
        speedM1 = 64
        speedM2 = 64
        speedM3 = 64
        if (theta<0):
            max_rps = -max_rps
        for i in range(0,int(tune_angle*theta)):
            self.driveM1(speedM1)
            self.driveM2(speedM2)
            self.driveM3(speedM3)
            kp_theta = 1
            speedM1 = speedM1 + kp_theta*(max_rps - self.readCurrentRpsM1())
            speedM2 = speedM2 + kp_theta*(max_rps - self.readCurrentRpsM2())
            speedM3 = speedM3 + kp_theta*(max_rps - self.readCurrentRpsM3())
            if i%10==0:
                print str(self.readCurrentRpsM1()) + "\t\t" + str(self.readCurrentRpsM2()) + "\t\t" + str(self.readCurrentRpsM3()) 
        self.stop()

    def goToPoint(self, max_rps,x,y):
        print "Moving to (x,y)=(" + str(x) + "," + str(y) + ")"
        tune_distance = 5
        distance = int(tune_distance*math.sqrt(x*x+y*y))
        rpsM1 = self.rpsComponentM1(max_rps,x,y)
        rpsM2 = self.rpsComponentM2(max_rps,x,y)
        rpsM3 = self.rpsComponentM3(max_rps,x,y)
        speedM1 = 64
        speedM2 = 64
        speedM3 = 64
        for i in range(0,distance):
            self.driveM1(speedM1)
            self.driveM2(speedM2)
            self.driveM3(speedM3)
            kp_xy = 1
            speedM1 = speedM1 + kp_xy*(rpsM1 - self.readCurrentRpsM1())
            speedM2 = speedM2 + kp_xy*(rpsM2 - self.readCurrentRpsM2())
            speedM3 = speedM3 + kp_xy*(rpsM3 - self.readCurrentRpsM3())
            if i%10==0:
                print str(self.readCurrentRpsM1()) + "\t\t" + str(self.readCurrentRpsM2()) + "\t\t" + str(self.readCurrentRpsM3())
        self.stop()

################# Implementation #####################

if __name__ == '__main__':
    theta = float(sys.argv[1])
    x = float(sys.argv[2])
    y = float(sys.argv[3])
    max_rps = 3

    n = Navigate()

    n.rotate(max_rps,theta)
    n.goToPoint(max_rps,x,y)






