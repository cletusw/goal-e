#!/usr/bin/python
import argparse
from client import Client
class Main:
    def __init__(self):
        self.parse_arguments()
    def parse_arguments(self):
        parser = argparse.ArgumentParser(prog='Client for robot', description='Client to send messages to other robot', add_help=True)
        parser.add_argument('-s', '--server', type=str, action='store', help='host', default="192.168.1.112")
        parser.add_argument('-p', '--port', type=str, action='store', help='port', default="6000")
        self.args = parser.parse_args()
    def run(self):
        c = Client(self.args.server, self.args.port)
        c.run()
if __name__ == "__main__":
    m = Main()
    m.parse_arguments()
    m.run()
