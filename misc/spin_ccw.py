# Wheels from when looking at the kicker, 
#	starting to the left and going clockwise
#	  Wheel		Board #		Board Section
#		A 	--    81	--		0
#		B 	--    80	--		1
#		C   --    80	--		0
from drive_motor import run_drive

# all are oriented clowckwise
def spin_ccw(): 
	run_drive("0x80","0","45000") #C
	run_drive("0x80","1","-30000") #B
	run_drive("0x81","0","15000") #A

spin_ccw()
