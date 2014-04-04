#include "ros/ros.h"
#include "goale/RangeBearing.h"

int main(int argc, char **argv) {
  ros::init(argc, argv, "vision");
  ros::NodeHandle n;

  ros::Publisher ballRangeBearing = n.advertise<goale::RangeBearing>("ballRangeBearing", 1000);
  
  ros::Rate loop_rate(10);

  float count = 0;
  while (ros::ok()) {
      goale::RangeBearing msg;

      msg.range = count;
      msg.bearing = -count + 1.2;

      ROS_INFO("range: %f, bearing: %f", msg.range, msg.bearing);

      ballRangeBearing.publish(msg);

      ros::spinOnce();

      loop_rate.sleep();
      ++count;
  }

  return 0;
}

