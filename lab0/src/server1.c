#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(){
    //create sockets one for listening, other one for tcp connection
    int server_fd, client_fd;
    struct sockaddr_in client_addr, server_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("Failed to create a socket");
        exit(EXIT_FAILURE);
    }
    //define server socket address to accept incoming connection on an interface
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; //accepting tcp connecitons from all interfaces
    //bind
    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }
    //listen
    if(listen(server_fd, 5)<0){
        perror("Failed to listen for incoming connections");
        exit(EXIT_FAILURE);
    }
    //accept
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if(client_fd<0){
        perror("Failed to accept incoming connection");
        exit(EXIT_FAILURE);
    }

    //read
    read(client_fd, buffer, BUFFER_SIZE);
    printf("Message from Client: %s", buffer);

    //send
    char* response = "Message Recieved\n";
    send(client_fd, response, strlen(response),0);

    //close
    close(client_fd);
    close(server_fd);
    return 0;
}