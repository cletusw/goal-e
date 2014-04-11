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
        rospy.init_node('wifi', anonymous=True)
        self.refereeTopic = rospy.Publisher('referee', String)
        self.r = rospy.Rate(10)
        while not rospy.is_shutdown():
            (client,address) = self.server.accept()
            self.handleClient(client)
    def handleClient(self,client):
        while True:
             data = client.recv(self.size)
             if data:
                #commands from referee
                if data[0] == "0": #all stop
                    self.refereeTopic.publish("stop")
                elif data[0] == "1": #start game
                    self.refereeTopic.publish("start")
                elif data[0] == "2": #reset game
                    self.refereeTopic.publish("reset")
             else:
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
       self.sleep()
