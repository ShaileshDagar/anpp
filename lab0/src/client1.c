#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(){
    //create a socket
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd<0){
        perror("Failed to create a socket");
        exit(EXIT_FAILURE);
    }

    //define server address
    memset(&server_addr, 0 , sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //connect
    if(connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        perror("Failed to connect to server");
        exit(EXIT_FAILURE);
    }

    //send
    char * message = "Hello from the client side\n";
    send(sock_fd, message, strlen(message),0 );

    //read response
    read(sock_fd, buffer, BUFFER_SIZE);
    printf("Reply from Server: %s\n", buffer);

    //close
    close(sock_fd);

    return 0;
}