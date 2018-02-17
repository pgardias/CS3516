To run the client on its own, first open the command prompt in the unzipped file, where http_client.py and 
http_server.py are located. To run an instance of the client, run the following command:

.\http_client.py [--ttl] WWW.EXAMPLE.COM PORT

The --ttl flag is optional, and will result in the RTT being printed after the GET request is made and filfilled. 
The url portion, here shown as WWW.EXAMPLE.COM can be changed to any other link of your choosing. The final parameter, 
PORT, should always be 80 if you are trying to receive a webpage from outside of the host machine. 

----

To run the client and server together on the same machine, open the command prompt in the same way as above. First 
run an instance of the server by running the following command:

.\http_server.py PORT

The only parameter for this command is the port which will be used by the server and client to communicate. The port 
value should be the same for both the server and the client, otherwise they will not be able to exchange information.
Once you run the server, it will provide information on the IP address of the host machine, which you will need to 
input as one of the parameters for the client. An example IP address message that the server will print when run is 
this:

192.168.56.1:5555
Waiting for connection...

The IP which will be used in the running of the client is 192.168.56.1, while the port which will  be used is 5555, in 
this case. This will vary depending on your machine and what you passed as the port for the server. To connect the client 
to the now running server, you should run the following command:

.\http_client.py [--ttl] XXX.XXX.XX.X PORT

Replace the XXX.XXX.XX.X with the IP that was given by the server and PORT with the port that the server is running on.

----

Notes:

1. The file which is sent to the client from the server is named 'TMDG.html' rather than the standard 'index.html'. If it is 
necessary, it can be easily changed to 'index.html' by renaming the file and changing the name in the following line of 
code within http_server.py:

TMDG = file.read(open("TMDG.html"))

2. The server can be run without the accompanying client and the html file can be requested and rendered by going to the IP 
which was given by the server in any browser (Chrome, IE, Safari, etc).