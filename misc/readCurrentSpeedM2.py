import serial
import sys
import param_vars

ser2 = serial.Serial('/dev/ttyPCH2',38400)

def serialSend2(send):
    for i in send:
        ser2.write(chr(i))

def serialRead2(read):
    serialSend2(read)
    data=[]
    for i in range(6):
        data.append(ser2.read())
    return data

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
    # Calculate the speed in rotations per second.
    rotations_per_second = float(speed) * 125 / 8192 
    print 'M2 Speed:\n' + str(rotations_per_second) 
    return rotations_per_second

readCurrentSpeedM2()
