#!/usr/bin/env python
import roslib
roslib.load_manifest('goale')
import rospy
import tf

class DriveSquare:
  direction = 0

  def runFrame(self):
    if self.direction == 0:
      rospy.loginfo("straight")
      self.direction = 1
    elif self.direction == 1:
      rospy.loginfo("left")
      self.direction = 2
    elif self.direction == 2:
      rospy.loginfo("back")
      self.direction = 3
    else:
      rospy.loginfo("right")
      self.direction = 0

if __name__ == '__main__':
  rospy.init_node('Ai')

  ai = DriveSquare()

  r = rospy.Rate(0.5) # 0.5hz
  while not rospy.is_shutdown():
    ai.runFrame()
    r.sleep()
