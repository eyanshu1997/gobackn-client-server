DEVELOPER DETAILS:
Eyanshu Gupta
2019H1030018P

AIM: 
To implement a client and a server program using DATAGRAM sockets. The
client sends a file (e.g., infile.txt) to the server using the modified Go-Back-N ARQ
protocol.

CONTENTS: 
client.c 
server.c

COMPILE CODE:
client: cc client.c -o c
server: cc server.c -o s

CLIENT EXECUTION ARGUMENTS:
The client takes 5 arguments namely:
ip of server , port, sourcefile name, packet size, window size.

EXAMPLE EXECUTION STATEMENTS:
Server: ./s
Client: ./c 127.0.0.1 8888 test.txt 10 5

SERVER PORT NO: 
The Server port no is precoded as 8888.

PACKET FORMAT : 
sequence no, packet type , end of file, packet length, data

PACKETIZATION PROCESS: 
Using fread and fseek function by setting file pointer 
at the position of next file part(ack no*size) and reading next no of 
characters or till end of file.

FILE WRITING PROCESS: 
file is being written using fputs function after getting
the data from the packet by removing and analyzing headers.

LIMITATIONS: 
The current version of this programs and sending the image files 
generates some error in the recieved file because of use of character buffers in c.

NOTABLE ADDITIONS:
The buffers are dynamically allocated according to packet size.
The client specifies the error if the server doesnt exist.
