#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080

int main(){
    //data structures
    int server_fd, client1_fd=0, client2_fd=0;
    struct sockaddr_in server_addr, client1_addr, client2_addr;
    socklen_t sock_len = sizeof(client1_fd);
    int client1_connected = 0, client2_connected = 0;
    char ch;

    //socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("Failed to create listening socket");
        exit(EXIT_FAILURE);
    }

    printf("Listening socket successfully created\n");

    //server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //bind
    if(bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
        perror("Failed to bind listening socket");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Successful binding of listening socket\n");
    printf("Listening on port %d\n", PORT);

    //listen
    if(listen(server_fd, 2) < 0){
        perror("Failed to listen for incoming connections");
        exit(EXIT_FAILURE);
    }

    //accept all
    while(!client1_connected  || !client2_connected){

        int temp_fd;
        struct sockaddr_in temp_addr;
        temp_fd = accept(server_fd, (struct sockaddr*) &temp_addr, &sock_len);
        char buf[8];

        if(temp_fd < 0){
            perror("Failed to accept incoming connection");
            if(client1_fd){
                close(client1_fd);
            }
            if(client2_fd){
                close(client2_fd);
            }
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        if(read(temp_fd, buf, 8) < 0){
            perror("Failed to read the identification string");
            if(client1_fd){
                close(client1_fd);
            }
            if(client2_fd){
                close(client2_fd);
            }
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        printf("Identification string received: %s", buf);

        int is_client1 = strcmp(buf, "client1\n");

        if(is_client1 == 0){
            client1_connected = 1;
            client1_fd = temp_fd;
            client1_addr = temp_addr;
            printf("Client 1 connected\n");
        }
        else{
            client2_connected = 1;
            client2_fd = temp_fd;
            client2_addr = temp_addr;
            printf("Client 2 connected\n");
        }
    }

    //read from client 1
    if(read(client1_fd, &ch, sizeof(ch)) < 0){
        perror("Failed to read from client 1");
        close(client1_fd);
        close(client2_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Character from client 1: %c\n", ch);

    //write to client 2
    if(send(client2_fd, &ch, sizeof(ch), 0) < 0){
        perror("Failed to send to client 2");
        close(client1_fd);
        close(client2_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Successfull sent character to client 2\n");

    //close
    close(client1_fd);
    close(client2_fd);
    close(server_fd);

    return 0;
}