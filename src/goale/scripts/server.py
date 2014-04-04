import socket
import os
import rospy
from std_msgs.msg import String
import sys
class Server:
    def __init__(self,port):
        self.host = ""
        self.port = port
        self.size = 1024
        self.open_socket()
    def open_socket(self):
        try:
            self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)
            self.server.bind((self.host,self.port))
            self.server.listen(5)
        except:
            if self.server:
                self.server.close()
            print "Could not open socket"
            sys.exit(1)
    def run(self):
        #spin up ROS
        self.myPositionFromOtherRobotTopic = rospy.Publisher('myPositionFromOtherRobot', String) 
        self.refereeTopic = rospy.Publisher('referee', String)
        self.strategyTopic = rospy.Publisher('strategy', String)
        rospy.init_node('wifi', anonymous=True)
        self.r = rospy.Rate(10)
        while not rospy.is_shutdown():
            while True:
                (client,address) = self.server.accept()
                self.handleClient(client)
    def handleClient(self,client):
        while True:
            data = client.recv(self.size)
            if data
                #commands from referee
                if data == "start":
                    os.system("python ~/driveAll.py 50")
                elif data == "goal":
                    os.system("python ~/returnToStart.py")
                elif data == "stop":
                    os.system("python ~/allStop.py")
                elif "myPosition" in data: #message from other robot
                    self.publishMyPosition(data)
                elif "runPlay" in data: #message from other robot
                    self.publishPlay(data)
                print "got it"
                client.send("hi")
            else:
                print "closing"
                client.close()
                break;
    def publishMyPosition(self, data):
       info = data.split("\s")
       stringToPublish = info[1] + "%s" %rospy.get_time()
       rospy.loginfo(stringToPublish)
       self.myPositionFromOtherRobotTopic.publish(stringToPublish)
       self.r.sleep()
   def publishPlay(self, data): 
       info = data.split("\s")
       stringToPublish = info[1] + "%s" %rospy.get_time()
       rospy.loginfo(stringToPublish)
       self.strategyTopic.publish(stringToPublish)
       self.r.sleep()
