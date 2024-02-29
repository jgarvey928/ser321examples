#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void exit_client(const char* msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]){

	//1) Fetch client parameters (server IP, server port, text message)
	if (argc != 4) {
	    exit_client("Parameter(s): <Server Address>, <Server Port>, <Message>");
    }

	printf(" ---- My Echo C Client ---- \n");
	// Server IP
	char* server_IP = argv[1];
	printf("Server IP : %s\n", server_IP);
	// Server Port Number
	char* port_num = argv[2];
	// Convert to Short (typedef in_port_t)
	in_port_t server_Port = atoi(port_num);
	printf("Server Port : %d\n", server_Port);
	// String to be Echoed by the Server
	char* echo_Text = argv[3];
	printf("Echo String : %s\n", echo_Text);
	
	// 2) Create a socket using TCP.
	// AF_INET for IPv4  AND  AF_INET6 for IPv6
	// SOCK_STREAM for two-way byte stream
	// IPPROTO_TCP for TCP, IPPROTO_UDP for UDP
	// Creates a socket a picks its own port number automatically
	int client_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if((client_Socket) < 0){
		exit_client("socket(): failed creation");
	}

	// 3) Construct the server address struct
	// struct sockaddr_in from <netinet/in.h>
	struct sockaddr_in server_Addr;
	//allocate memory for the struct
	memset(&server_Addr, 0, sizeof(server_Addr));
	server_Addr.sin_family = AF_INET;
	//converts server ip from string to numeric struct form in server_addr.sin_addr.s_addr
	int rtnVal = inet_pton(AF_INET, server_IP, &server_Addr.sin_addr.s_addr); 
	if (rtnVal == 0) {
	    exit_client("inet_pton(): invalid ip address");
    } else if (rtnVal < 0) {
	    exit_client("inet_pton(): error when parsing");
    }
	//converts port number (host to network short) to correct bit ordering (big-endian)
	server_Addr.sin_port = htons(server_Port); 

    // 4) Establish the connection to the server.
	rtnVal = connect(client_Socket, (struct sockaddr *) &server_Addr, sizeof(server_Addr));
	if (rtnVal < 0) {
	  exit_client("connect(): failed");
    }

	// 5) Send message.
	size_t echo_Text_Len = strlen(echo_Text);
	ssize_t numBytes = send(client_Socket, echo_Text, echo_Text_Len, 0);
	if (numBytes < 0) {
	  exit_client("send(): failed to send message");
    } else if (numBytes != echo_Text_Len) {
	  exit_client("send(): sent unexpected number of bytes");
    }

    // 6) Receive message.
	unsigned int bytes_recieved = 0;
	printf("Received: ");
	while(bytes_recieved < echo_Text_Len){
		char buffer[BUFFER_SIZE];
		numBytes = recv(client_Socket, buffer, BUFFER_SIZE - 1, 0);
	    if (numBytes < 0) {
	        exit_client("recv(): failed");
        } else if (numBytes == 0) {
	        exit_client("recv(): connection closed");
		}
		buffer[numBytes] = '\0';
		bytes_recieved += numBytes;
		// fputs(buffer, stdout);
		printf("%s\n", buffer);
	}
	// printf("\n");

	//7) Close the socket.
	close(client_Socket);
	exit(0);

}