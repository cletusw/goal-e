#!/usr/bin/env python
import rospy
from goale.msg import RangeBearing 

def talker():
  pub = rospy.Publisher('ballRangeBearing', RangeBearing)
  rospy.init_node('FakeBall')
  r = rospy.Rate(10) # 10hz
  maxBearing = 5.0
  bearing = -maxBearing
  diff = 0.1
  while not rospy.is_shutdown():
    bearing = bearing + diff
    if (diff > 0 and abs(bearing - maxBearing) < 0.1) or (diff < 0 and abs(bearing + maxBearing) < 0.1):
      diff = -diff
    msg = RangeBearing(2.0, bearing)
    rospy.loginfo(1)
    pub.publish(msg)
    r.sleep()

if __name__ == '__main__':
  try:
    talker()
  except rospy.ROSInterruptException: pass
