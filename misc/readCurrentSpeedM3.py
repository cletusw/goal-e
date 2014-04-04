import serial
import sys
import param_vars

ser1 = serial.Serial('/dev/ttyPCH1',38400)

def serialSend1(send):
    for i in send:
        ser1.write(chr(i))

def serialRead1(read):
    serialSend1(read)
    data=[]
    for i in range(6):
        data.append(ser1.read())
    return data

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
    # Calculate the speed in rotations per second.
    rotations_per_second = float(speed) * 125 / 8192 
    print 'M3 Speed:\n' + str(rotations_per_second) 
    return rotations_per_second

readCurrentSpeedM3()
