#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(){
    //data structures
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t client_addr_len = sizeof(client_addr);

    //listening socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
    printf("Listening Socket Created!\n");

    //define server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //bind
    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket Bind Successful!\n");

    //listen
    if(listen(server_fd, 5) < 0){
        perror("Failed to listen for incoming connections");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port: %d\n", PORT);

    //accept
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if(client_fd < 0){
        perror("Failed to accept incoming connection");
        exit(EXIT_FAILURE);
    }

    printf("Accepted incoming connection!\n");

    //read
    read(client_fd, buffer, BUFFER_SIZE);
    printf("Message from client: %s\n", buffer);

    //write a response
    char* response = "Message recieved!";
    send(client_fd, response, strlen(response), 0);

    //close
    close(client_fd);
    close(server_fd);

    return 0;
}