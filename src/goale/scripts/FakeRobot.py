#!/usr/bin/env python  
import roslib
roslib.load_manifest('goale')

import rospy
import tf

if __name__ == '__main__':
  rospy.init_node('FakeRobot')
  br = tf.TransformBroadcaster()
  rate = rospy.Rate(10.0)
  while not rospy.is_shutdown():
    br.sendTransform((0.0, 2.0, 0.0),
             (0.0, 0.0, 0.0, 1.0),
             rospy.Time.now(),
             "robot",
             "world")
    rate.sleep()