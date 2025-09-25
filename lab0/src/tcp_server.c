#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(){

    //create a socket
    int server_fd, client_fd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("Failed to create a socket to listen for incoming connections");
        exit(EXIT_FAILURE);
    }

    //bind socket to listen for connection on particular interface and port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; //0.0.0.0

    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }
    
    //start listening for incoming connections
    if(listen(server_fd, 5)<0){
        perror("Failed to Listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    //accept
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if(client_fd<0){
        perror("Failed to accept incoming connection");
        exit(EXIT_FAILURE);
    }

    //read
    read(client_fd, &buffer, BUFFER_SIZE);
    printf("Message from Client: %s", buffer);

    //write a response
    char* response = "I got your message!";
    send(client_fd, response, strlen(response), 0);

    //close
    close(client_fd);
    close(server_fd);
}