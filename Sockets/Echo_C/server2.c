#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define RCVBUFSIZE 32
static const int MAXPENDING = 5;

void exit_client(char* msg) {
	perror(msg);
	exit(1);
}

void handle_client_socket(int clntSocket);

int main(int argc, char *argv[]) {
    
    // 1) Fetch server parameters (server port).
	if (argc != 2) {
		exit_client("Parameter(s): <Server Port>");
    }
    //in_port_t is actually equivalent to uint16_t! (in.h)
    in_port_t server_port = atoi(argv[1]);

	// 2) Create a socket using TCP.
    //socket(domain, socket type (byte-stream), protocol)
    // protocol macro defined in in.h, others in socket.h
	int server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_sock < 0) { 										
		exit_client("socket(): failed creation");
    }

	//3) Construct the server address struct
    //struct defined in netinet/in.h
	struct sockaddr_in server_addr; 
    //allocate memory for the struct
	memset(&server_addr, 0, sizeof(server_addr));
    //assign necessary values to the struct (netinet/in.h)
	server_addr.sin_family = AF_INET;
    //sin_addr is actually another struct, we are assigning the value to it's s_addr field!
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //host to network short and host to network long convert the values to big-endian ordering (MSB first) for compatibility
	server_addr.sin_port = htons(server_port); 

    // 4) Bind to the local address (Java takes care of this step for you)
    //bind(socket, socketaddress, socket address length)
    int rtnVal = bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if (rtnVal < 0) { 
       exit_client("bind() failed");
    }

    // 5) Mark the socket so it will listen for incoming connections
    rtnVal = listen(server_sock, MAXPENDING);
	if (rtnVal < 0) {
	    exit_client("listen() failed");
    }

    //infinite loop begins
	for (;;) {
        //struct to store the clients address
        struct sockaddr_in client_addr;
        //store clients address length
        socklen_t client_addr_len = sizeof(client_addr);

        // 6) Waits for a client to connect (accept() blocks)
        //creates client socket from listening queue with accept(server socket, client address, client address length)
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock < 0) {
            exit_client("accept() failed");
        }

        // 7) Handle client connection.
        //macro in in.h (16 characters)
        char client_name[INET_ADDRSTRLEN];
        //inet_ntop() converts the numeric address into a text string called client name
        if ( inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_name, sizeof(client_name)) != NULL ){
            //network to host short conversion
            printf("Handling client %s/%d\n", client_name, ntohs(client_addr.sin_port));
        } else {
            printf("Unable to get client address\n");
        }
        handle_client_socket(client_sock);
    }
    //Never reaches here!
}

void handle_client_socket(int client_sock) {
    //defined in this file as 32 characters
    char buffer[RCVBUFSIZE];

    //1) Receive message.
    ssize_t numBytesRcvd = recv(client_sock, buffer, RCVBUFSIZE, 0);
    printf("%s\n", buffer);
    if (numBytesRcvd < 0) {
        exit_client("recv() failed");
    }

    //2) Reply by sending back the message.
    ssize_t numBytesSent = send(client_sock, buffer, numBytesRcvd, 0);
    if (numBytesSent < 0) {
        exit_client("send() failed");
    }
    else if (numBytesSent != numBytesRcvd) {
        exit_client("send(): sent unexpected number of bytes");
    }

    //4) Close the socket.
    close(client_sock);
}