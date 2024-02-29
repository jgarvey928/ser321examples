#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

static const int MAXPENDING = 5;

void handle_client_socket(int clntSocket);

int main(int argc, char *argv[]) {
    
    // 1) Fetch server parameters (server port).
    // "Parameter(s): <Server Port>"
    in_port_t server_port = atoi(argv[1]);

    // 2) Create a socket using TCP.
    int server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //3) Construct the server address struct
    struct sockaddr_in server_addr; 
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(server_port); 

    // 4) Bind to the local address (Java takes care of this step for you)
    bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr));

    // 5) Mark the socket so it will listen for incoming connections
    listen(server_sock, MAXPENDING);

    //infinite loop begins
	for (;;) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        // 6) Waits for a client to connect (accept() blocks)
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);

        // 7) Handle client connection.
        char client_name[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_name, sizeof(client_name));
        printf("Handling client %s/%d\n", client_name, ntohs(client_addr.sin_port));

        handle_client_socket(client_sock);  
    }
    //Never reaches here!
}


void handle_client_socket(int client_sock) {

    //1) Receive message.
    char buffer[32];
    ssize_t numBytesRcvd = recv(client_sock, buffer, 32, 0);
    printf("Echo : %s\n", buffer);
    while (numBytesRcvd > 0) {
        //2) Reply by sending back the message.
        ssize_t numBytesSent = send(client_sock, buffer, numBytesRcvd, 0);
        //3) Receive message (if exists).
        numBytesRcvd = recv(client_sock, buffer, 32, 0);
        printf("Echo : %s\n", buffer);
    }
    //4) Close the socket.
    close(client_sock);
}