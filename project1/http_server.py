import socket
import sys
import thread
import select
import time

PORT = int(sys.argv[1])
HOST = socket.gethostbyname(socket.gethostname())
BUFFER_SIZE = 4096
TMDG = file.read(open("TMDG.html"))
print str(HOST) + ":" + str(PORT)

def handler(conn_s, ip):
    data = ""
    message = ""

    while 1:
        active = select.select([conn_s], [], [], .01)
        if active[0]:
            data = conn_s.recv(BUFFER_SIZE)
        if not active[0]:
            break
        if data:
            message += data
        if not data:
            break
    if "GET /TMDG.html HTTP/1.1" in message or "GET / HTTP/1.1" in message:
        conn_s.send(
            "HTTP/1.1 200 OK\r\nDate: " + time.ctime() + "\r\nServer: Python/2.7.11\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" + TMDG)
        print "Requested file sent"
    else:
        conn_s.send(
            "HTTP/1.1 404 Not Found\r\nDate: " + time.ctime() + "\r\nServer: Python/2.7.11\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n")
        print "Requested file not found"
    conn_s.close()


addr = (HOST, PORT)
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except socket.gaierror as error:
    print "Socket creation failed with error %s\n" % error
    exit(-1)

s.bind(addr)
s.listen(10)
while 1:
    print "Waiting for connection...\r"
    c, client_ip = s.accept()
    print "Connected to", addr, "\r"
    thread.start_new_thread(handler, (c, client_ip))
