#!/usr/bin/env python
import roslib
roslib.load_manifest('goale')

import rospy

if __name__ == '__main__':
  rospy.init_node('StateEstimation')
  br = tf.TransformBroadcaster()
  rate = rospy.Rate(10.0)
  while not rospy.is_shutdown():
    t = rospy.Time.now().to_sec()
    rospy.loginfo('%d',t)
    br.sendTransform((1.0, 1.0, 0.0),
      (0.0, 0.0, 0.0, 1.0),
      rospy.Time.now(),
      '
