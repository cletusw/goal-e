#!/usr/bin/env python
# license removed for brevity
import rospy
from goale.msg import RangeBearing 

def talker():
    pub = rospy.Publisher('chatter', RangeBearing)
    rospy.init_node('talker', anonymous=True)
    r = rospy.Rate(10) # 10hz
    while not rospy.is_shutdown():
        msg = RangeBearing(1.2, -3.4)
        rospy.loginfo(1)
        pub.publish(msg)
        r.sleep()

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException: pass
