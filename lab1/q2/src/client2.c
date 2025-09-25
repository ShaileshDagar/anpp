#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_PORT 8080
#define LOOPBACK "127.0.0.1"

int main(){
    //data structures
    int sock_fd;
    struct sockaddr_in server_addr;
    float f;

    //create a socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0){
        perror("Failed to create a socket");
        exit(EXIT_FAILURE);
    }

    printf("Socket successfully created for client 2\n");

    //define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(LOOPBACK);

    //connect to server
    if(connect(sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
        perror("Client 2 failed to connect to server");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("Successfully connected to the server\n");

    //send identification string
    char* id = "client2\n";

    if(send(sock_fd, id, strlen(id), 0) < 0){
        perror("Failed to send identification string");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("ID String successfully sent\n");

    //recieve data from server
    if(read(sock_fd, &f, sizeof(f)) != sizeof(f)){
        perror("Client 2 failed to recieve data from server");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Successfully recieved data from server: %f\n", f);

    //close
    close(sock_fd);

    return 0;
}