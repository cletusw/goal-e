import sys
import os
import time

speed = sys.argv[1]
t = sys.argv[2] #time

os.system('python driveM1.py -%s'% speed)
os.system('python driveM3.py %s'% speed)
time.sleep(float(t))
os.system('python driveM1.py 0')
os.system('python driveM3.py 0')

