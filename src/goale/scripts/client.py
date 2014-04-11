import socket
import sys
class Client:
    def __init__(self,host,port):
        self.host = host
        self.port = port
        self.size = 1024
        self.open_socket()
    def open_socket(self):
        try:
            self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server.connect((self.host, self.port))
        except socket.error, (value,message):
            if self.server:
                self.server.close()
            print "Could not open socket: " + message
            sys.exit(0)
    def run(self):
        rospy.init_node('clientWifi', anonymous=True)
        rospy.Subscriber("referee", String, sendMessage)
        rospy.spin()
    def sendMessage(self, data): 
        print "data from referee topic: ",data.data
        #self.server.send(data.data)
        #dataRecv = self.server.recv(self.size)
        #self.server.close()
