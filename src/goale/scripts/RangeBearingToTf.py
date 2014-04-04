#!/usr/bin/env python
import roslib
roslib.load_manifest('goale')
import rospy
import math
import tf
import geometry_msgs.msg
from goale.msg import RangeBearing

def callback(rangeBearing):
  rospy.loginfo(rangeBearing)
  x = rangeBearing.range * math.sin(rangeBearing.bearing)
  y = rangeBearing.range * math.cos(rangeBearing.bearing)

  br = tf.TransformBroadcaster()
  br.sendTransform((x, y, 0),
    (0.0, 0.0, 0.0, 1.0),
    rospy.Time.now(),
    'ball',
    'world')

if __name__ == '__main__':
  rospy.init_node('RangeBearingToTf', anonymous=True)
  rospy.Subscriber('ballRangeBearing', RangeBearing, callback)
  rospy.spin()
