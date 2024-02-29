#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){

	//1) Fetch client parameters (server IP, server port, text message)
    // "Parameter(s): <Server Address>, <Server Port>, <Message>"
	char* server_IP = argv[1];
	char* port_num = argv[2];
	in_port_t server_Port = atoi(port_num);
	char* echo_Text = argv[3];
	
	// 2) Create a socket using TCP.
	int client_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 3) Construct the server address struct
	struct sockaddr_in server_Addr;
	memset(&server_Addr, 0, sizeof(server_Addr));
	server_Addr.sin_family = AF_INET;
	inet_pton(AF_INET, server_IP, &server_Addr.sin_addr.s_addr); 
    server_Addr.sin_port = htons(server_Port); 

    // 4) Establish the connection to the server.
	connect(client_Socket, (struct sockaddr *) &server_Addr, sizeof(server_Addr));

	// 5) Send message.
	size_t echo_Text_Len = strlen(echo_Text);
	ssize_t numBytes = send(client_Socket, echo_Text, echo_Text_Len, 0);

    // 6) Receive message.
	unsigned int bytes_recieved = 0;
    char buffer[1024];
	numBytes = recv(client_Socket, buffer, 1024, 0);
    buffer[numBytes] = '\0';
    printf("Echo: %s\n", buffer);

	//7) Close the socket.
	close(client_Socket);
	exit(0);

}