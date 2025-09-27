#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#define MAX_ELEMS 128
#define PORT 12345
#define MAX_UDP_PAYLOAD 1500

static void hexdump(const uint8_t* buf, size_t len){
    for(size_t i=0; i<len; i++){
        if(i%16 == 0) printf("%04zu: ", i);
        printf("%02x ", buf[i]);
        if(i%16 == 115 || i == len-1) printf("\n");
    }
}

int process_packet(int sock, const uint8_t* buf, ssize_t len, struct sockaddr_in* cli, socklen_t cli_len){
    const uint8_t* p = buf;

    size_t s_len = sizeof(uint16_t);
    size_t l_len = sizeof(int32_t);
    uint16_t n1_net, n2_net;
    memcpy(&n1_net, p, s_len);
    p += s_len;
    memcpy(&n2_net, p, s_len);
    p += s_len;
    uint16_t n1 = ntohs(n1_net);
    uint16_t n2 = ntohs(n2_net);

    if(n1 > MAX_ELEMS || n2 > MAX_ELEMS){
        perror("Number of elements exceed the limit.");
        return 0;
    }

    size_t expected = 2*s_len + l_len*((size_t)n1 + (size_t)n2);
    if((size_t)len != expected){
        printf("n1 = %u, n2 = %u, expected = %zu, recieved = %zu\n", n1, n2, expected, len);
        perror("Size mismatch between buffer and the size indicated by number of elements");
        return 0;
    }

    int invalid=0;
    int64_t sum1=0, sum2=0;

    for(uint16_t i=0; i<n1; i++){
        uint32_t tmp;
        memcpy(&tmp, p, l_len);
        p += l_len;
        int32_t val = ntohl(tmp);

        if((uint32_t)val & 1u != 0u){
            printf("Odd number found in array arr1[%u] = %d", i, val);
            invalid = 1;
            break;
        }
        sum1 += (int64_t)val;
    }

    if(!invalid){
        for(uint16_t i=0; i<n2; i++){
            int32_t tmp;
            memcpy(&tmp, p, l_len);
            p += l_len;
            int32_t val = ntohl(tmp);
            if((uint32_t)val & 1u != 0u){
                printf("Odd number found in array arr2[%u] = %d", i, val);
                invalid = 1;
                break;
            }
            sum2 += (int64_t)val;
        }
    }

    if(invalid){
        fprintf(stderr, "Packet from %s:%d rejected because of odd value in array.\n", inet_ntoa(cli->sin_addr), ntohs(cli->sin_port));
        return 0;
    }

    if(sum1 < INT32_MIN || sum1 > INT32_MAX || sum2 < INT32_MIN || sum2 > INT32_MAX){
        fprintf(stderr, "sum overflow: sum1 = %lld sum2 = %lld\n", (long long)sum1, (long long)sum2);
        return 0;
    }

    int32_t s1 = (int32_t)sum1;
    int32_t s2 = (int32_t)sum2;

    uint32_t s1_net = htonl((uint32_t)s1);
    uint32_t s2_net = htonl((uint32_t)s2);

    uint8_t reply[2*l_len];
    memcpy(reply, &s1_net, l_len);
    memcpy(reply + l_len, &s2_net, l_len);

    hexdump(reply, sizeof(reply));

    ssize_t sent = sendto(sock, reply, sizeof(reply), 0, (struct sockaddr*)cli, cli_len);
    if(sent != sizeof(reply)){
        perror("Error sending reply using sendto");
        return -1;
    }

    printf("Reply sent to %s:%d sum1 = %d sum2 %d\n", inet_ntoa(cli->sin_addr), ntohs(cli->sin_port), s1, s2);
    return 0;
}

int main(){

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if(sock<0){
        perror("Failed to create a UDP socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in srv;
    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_port = htons((uint16_t)PORT);
    srv.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind to port
    if(bind(sock, (struct sockaddr*)&srv, sizeof(srv)) < 0){
        perror("Error bind sock to port");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening on port %d\n", PORT);

    uint8_t buffer[MAX_UDP_PAYLOAD];
    while(1){
        struct sockaddr_in cli;
        socklen_t cli_len = sizeof(cli);

        //recvfrom

        ssize_t n = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&cli, &cli_len);
        if(n < 0){
            if(errno == EINTR) continue;
            perror("recvfrom failed");
            close(sock);
            exit(EXIT_FAILURE);
        }

        printf("Recieved %zd bytes from %s:%d\n", n, inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));

        //hexdump of data recieved
        hexdump(buffer,(size_t)n);
        
        //process packet
        if(process_packet(sock, buffer, n, &cli, cli_len) < 0){
            printf("could not send a reply\n");
            close(sock);
            exit(EXIT_FAILURE);
        }
    }

    close(sock);
    return 0;
}