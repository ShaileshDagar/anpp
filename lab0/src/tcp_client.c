// struct in_addr {
//     in_addr_t s_addr;            // IPv4 address (32 bits), in network byte order
// };

// struct sockaddr_in {
    // sa_family_t    sin_family;   // address family: AF_INET
    // in_port_t      sin_port;     // port number, must be in network byte order
    // struct in_addr sin_addr;     // IP address
    // char           sin_zero[8];  // padding to match `struct sockaddr` size
// };
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(){

    //create a socket

    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd<0){
        perror("Socket Creation failed!");
        exit(EXIT_FAILURE);
    }

    //Define Server Address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //connect to server
    if(connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        perror("TCP connection could not be established with the server");
        exit(EXIT_FAILURE);
    }

    //write to server
    char* message = "Hello from the other side!\n";
    send(sock_fd, message, strlen(message), 0);

    //read reply
    read(sock_fd, buffer, BUFFER_SIZE);
    printf("Server reply: %s\n", buffer);

    //close
    close(sock_fd);
}
