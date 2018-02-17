import socket
import select
import time
import sys

if len(sys.argv) > 3:  # --ttl flag included
    HOST = sys.argv[2]
    PORT = int(sys.argv[3])
else:
    HOST = sys.argv[1]
    PORT = int(sys.argv[2])
BUFFER_SIZE = 4096
HOST_IP = ""

HOST_NAME_AND_PATH = HOST.split("/", 1)
HOST_NAME = HOST_NAME_AND_PATH[0]
if len(HOST_NAME_AND_PATH) > 1:
    HOST_PATH = "/" + HOST_NAME_AND_PATH[1]
else:
    HOST_PATH = "/"

try:
    HOST_IP = socket.gethostbyname(HOST_NAME)
except socket.gaierror as error:
    print "Error resolving host with error %s\n" % error
    exit(-1)

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except socket.error as error:
    print "Socket creation failed with error %s\n" % error
    exit(-2)

s.connect((HOST_IP, PORT))
send_time = time.time()
s.send("GET %s HTTP/1.1\r\nHost: %s\r\nAccept: text/html, application/xhtml+xml, application/xml;q=0.9, */*;q=0.8 \
\r\nAccept-Language: en-us\r\n\r\n" % (HOST_PATH, HOST_NAME))
print "GET message sent to %s\n" % (HOST_NAME + HOST_PATH)

response = ""
data = ""
s.setblocking(0)
while 1:
    ready = select.select([s], [], [], .5)
    if ready[0]:
        data = s.recv(BUFFER_SIZE)
    if not ready[0]:
        break
    if not data:
        break
    if data:
        response += data

recv_time = time.time()
s.close()

print response, "\n"
if len(sys.argv) > 3:  # --ttl flag included
    print "%s ms\r\n" % (1000 * (recv_time - send_time))  # ms
