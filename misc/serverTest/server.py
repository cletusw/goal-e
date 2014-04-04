import socket
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
        while True:
            (client,address) = self.server.accept()
            self.handleClient(client)
    def handleClient(self,client):
        while True:
            data = client.recv(self.size)
            if data:
                print "got it"
                client.send("hi")
            else:
                print "closing"
                client.close()
                break;
               
