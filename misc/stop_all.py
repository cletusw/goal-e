# Wheels from when looking at the kicker, 
#	starting to the left and going clockwise
#	  Wheel		Board #		Board Section
#		A 	--    81	--		0
#		B 	--    80	--		1
#		C   --    80	--		0
from drive_motor import run_drive

# all are oriented clowckwise
def stop_all(): 
	run_drive("0x80","0","0") #C
	run_drive("0x80","1","0") #B
	run_drive("0x81","0","0") #A

stop_all()
