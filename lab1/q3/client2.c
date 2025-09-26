#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdint.h>
#include <sys/time.h>

#define MAX_ELEMS 128
#define MAX_UDP_PAYLOAD 1500
#define TIMEOUT_SEC 3
#define MAX_RETRIES 5
#define LOOPBACK "127.0.0.1"
#define PORT 12345

typedef struct{
    uint16_t n1;
    uint16_t n2;
    int32_t* arr1;
    int32_t* arr2;
}client_arrays_t;

size_t pack_client_arrays(const client_arrays_t* m, uint8_t* buf, size_t buflen){
    if(!m || !buf){
        perror("NULL value for either data structure or buffer");
        return 0;
    }
    size_t s_len = sizeof(uint16_t);
    size_t l_len = sizeof(int32_t);

    size_t needed = 2*s_len + l_len*((size_t)m->n1 + (size_t)m->n2);
    if(needed > buflen){
        perror("Buffer Overflow");
        return 0;
    }
    uint8_t* p = buf;

    uint16_t net_n1 = htons(m->n1);
    uint16_t net_n2 = htonl(m->n2);

    memcpy(p, &net_n1, s_len);
    p += s_len;
    memcpy(p, &net_n2, s_len);
    p += s_len;

    for(uint16_t i=0; i<m->n1; i++){
        int32_t v = htonl(m->arr1[i]);
        memcpy(p, &v, l_len);
        p += l_len;
    }
    for(uint16_t i=0; i<m->n2; i++){
        int32_t u = htonl(m->arr2[i]);
        memcpy(p, &u, l_len);
        p += l_len;
    }
    return needed;
}

int receive_reply(int sock){
    uint8_t buffer[MAX_UDP_PAYLOAD];
    struct sockaddr_in src;
    socklen_t src_len = sizeof(src);

    ssize_t n = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&src, &src_len);

    if(n < 0){
        perror("recvfrom failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if(n < 2*sizeof(int32_t)){
        perror("Shorter Response than expected");
        close(sock);
        exit(EXIT_FAILURE);
    }

    const uint8_t* p = buffer;
    uint32_t v1, v2;

    size_t len = sizeof(uint32_t);
    memcpy(p, &v1, len);
    p += len;
    memcpy(p, &v2, len);
    p += len;

    int32_t sum1 = ntohl(v1);
    int32_t sum2 = ntohl(v2);

    printf("Received response from server\n");
    printf("Sum of arr1 = %d\n", sum1);
    printf("Sum of arr2 = %d", sum2);

    return 0;
}

int main(){
    int32_t a1[] = {2, 4, 6, 8};
    int32_t a2[] = {10, 14, 16};

    client_arrays_t msg = {
        .n1 = sizeof(a1)/sizeof(a1[0]),
        .n2 = sizeof(a2)/sizeof(a2[0]),
        .arr1 = a1,
        .arr2 = a2
    };

    uint8_t buffer[MAX_UDP_PAYLOAD];
    size_t packed = pack_client_arrays(&msg, buffer, sizeof(buffer));
    
    if(packed == 0){
        exit(EXIT_FAILURE);
    }

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        perror("Failed to create a socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in srv;
    memset(&srv, 0 , sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    if(inet_pton(AF_INET, LOOPBACK, &srv.sin_addr) < 0){
        perror("inet_pton");
        close(sock);
        exit(EXIT_FAILURE);
    }

    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;

    if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv) < 0)){
        perror("Couldn't set timeout using setsockopt");
        close(sock);
        exit(EXIT_FAILURE);
    }

    int attempt;
    for(attempt=1; attempt <= MAX_RETRIES; attempt++){
        ssize_t sent = sendto(sock, buffer, packed, 0, (struct sockaddr*)&srv, sizeof(srv));

        if(sent < 0){
            perror("Sending failed at sendto");
            close(sock);
            exit(EXIT_FAILURE);
        }

        printf("Attempt %d: Sent %zd bytes to %s:%d\n", attempt, sent, LOOPBACK, PORT);

        if(receive_reply(sock) == 0) break;
        else printf("Timeout or error, retrying...");
    
    }

    if(attempt < MAX_RETRIES){
        printf("Server did not respond after %d attempts.\n", MAX_RETRIES);
    }

    close(sock);
    return 0;
}