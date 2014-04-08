#!/usr/bin/env python
import roslib
roslib.load_manifest('goale')
import rospy
import math
import tf
from geometry_msgs.msg import PointStamped
from goale.msg import RangeBearing

class RangeBearingToTf:
  objectFrame = ''
  sourceFrame = ''
  destinationFrame = ''
  listener = None
  broadcaster = None

  def __init__(self, objectFrame, sourceFrame, destinationFrame='world'):
    self.objectFrame = objectFrame
    self.sourceFrame = sourceFrame
    self.destinationFrame = destinationFrame
    self.listener = tf.TransformListener()
    self.broadcaster = tf.TransformBroadcaster()

    while not rospy.is_shutdown():
      try:
        self.listener.waitForTransform(self.destinationFrame, self.sourceFrame, rospy.Time(), rospy.Duration(1.0))
      except (tf.Exception):
        continue
      break

  def convertRangeBearing(self, rangeBearing):
    ps = PointStamped()
    ps.header.frame_id = self.sourceFrame
    ps.point.x = rangeBearing.range * math.sin(rangeBearing.bearing)
    ps.point.y = rangeBearing.range * math.cos(rangeBearing.bearing)
    ps.point.z = 0.0
    psInWorld = self.listener.transformPoint(self.destinationFrame, ps)
    
    self.broadcaster.sendTransform((psInWorld.point.x, psInWorld.point.y, 0),
      (0.0, 0.0, 0.0, 1.0),
      rospy.Time.now(),
      self.objectFrame,
      self.destinationFrame)

if __name__ == '__main__':
  rospy.init_node('RangeBearingToTf', anonymous=True)

  converter = RangeBearingToTf('ball', 'robot')

  rospy.Subscriber('ballRangeBearing', RangeBearing, converter.convertRangeBearing)
  rospy.spin()
