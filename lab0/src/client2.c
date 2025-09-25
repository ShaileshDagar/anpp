#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PORT 8080

int main(){

    char buffer[BUFFER_SIZE];

    //create a socket
    int sock_fd;
    struct sockaddr_in server_addr;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd<0){
        perror("failed to create socket");
        exit(EXIT_FAILURE);
    }

    printf("Successfully created Socket\n");

    //define sender address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //connect
    if(connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("Failed to connect to server");
        exit(EXIT_FAILURE);
    }

    printf("Connected to Server\n");

    //send
    char* message = "Hello! from the client side";
    send(sock_fd, message, strlen(message), 0);

    printf("Message sent %s\n", message);

    //recieve
    read(sock_fd, buffer, BUFFER_SIZE);
    printf("Response from server: %s\n", buffer);

    //close
    close(sock_fd);

    return 0;
}