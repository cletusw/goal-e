import argparse
from server import Server
class Main:
    def __init__(self):
        self.parse_arguments()

    def parse_arguments(self):
        parser = argparse.ArgumentParser(prog='EchoServer', description='testing robot soccer', add_help=True)
        parser.add_argument('-p', '--port', type=int, action='store', help='port to connect to', default=6000)
        self.args = parser.parse_args()
    def run(self):
        s = Server(self.args.port)
        s.run()
if __name__ == "__main__":
    m = Main()
    m.parse_arguments()
    try:
        m.run()
    except KeyboardInterrupt:
        pass
