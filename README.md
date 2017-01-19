# dns_tunnel
A program to tunnel data between a server and a client using DNS protocol and UDP sockets.

## Examples
The server takes listening address and port as parameters: ./server <listen_ip> <port>, and the client takes the server address and port as parameters: ./client <server_ip> <server_port>.

The tool will be used like netcat, except that there will be separate binaries for the server and the client. An example usage on localhost for linux could be for example:

$./server 127.0.0.1 53 > file &

$cat file | ./client 127.0.0.1 53

or the other way around:

$cat file | ./server 127.0.0.1 53 &

$./client 127.0.0.1 53 > file
