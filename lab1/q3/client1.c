#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdint.h>
#include <string.h>

#define MAX_ARRAY_ELEMS 128
#define MAX_UDP_PAYLOAD 1500

typedef struct{
    uint16_t n1;
    uint16_t n2;
    int32_t* arr1;
    int32_t* arr2;
}client_arrays_t;

size_t pack_client_arrays(const client_arrays_t* m, uint8_t* buf, size_t buflen){
    if(!m || !buf){
        perror("Either Client Structure Pointer or Buffer pointer is null");
        return 0;
    }
    if(m->n1 > MAX_ARRAY_ELEMS || m->n2 > MAX_ARRAY_ELEMS){
        perror("Number of elements are too big for UDP Payload");
        return 0;
    }

    size_t s_len = sizeof(uint16_t);
    size_t l_len = sizeof(int32_t);
    size_t needed = 2 * s_len + l_len * ((size_t)m->n1 + (size_t) m->n2);
    
    if(buflen < needed){
        perror("Data is larger than buffer");
        return 0;
    }

    uint8_t* p = buf;

    uint16_t n1_net = htons(m->n1);
    uint16_t n2_net = htons(m->n2);

    memcpy(p, n1_net, s_len);
    p += s_len;
    memcpy(p, n2_net, s_len);
    p += s_len;

    for(uint16_t i=0; i < m->n1; i++){
        uint32_t v = htonl(m->arr1[i]);
        memcpy(p, &v, l_len);
        p += l_len;
    }
    for(uint16_t i=0; i < m->n2; i++){
        uint32_t u = htonl(m->arr2[i]);
        memcpy(p, &u, l_len);
        p += l_len;
    }
    return needed;
}

int main(){
    client_arrays_t msg = {0};

    int32_t a1[] = {2, 4, 6, 8};
    int32_t a2[] = {10, 12, 14};

    msg.n1 = (uint16_t)sizeof(a1)/sizeof(a1[0]);
    msg.n2 = (uint16_t)sizeof(a2)/sizeof(a2[0]);

    msg.arr1 = malloc(sizeof(int32_t) * msg.n1);
    msg.arr2 = malloc(sizeof(int32_t) * msg.n2);

    if(!msg.arr1 || !msg.arr2){
        perror("Malloc failed");
        exit(EXIT_FAILURE);
    }

    memcpy(msg.arr1, a1, sizeof(a1));
    memcpy(msg.arr2, a2, sizeof(a2));

    uint8_t buffer[MAX_UDP_PAYLOAD];
    size_t packed = pack_client_arrays(&msg, buffer, sizeof(buffer));
    if(packed == 0){
        free(msg.arr1);
        free(msg.arr2);
        exit(EXIT_FAILURE);
    }

    printf("Packed %zu bytes ready to send via UDP.\n", packed);
    printf("On wire layout: n1 = %u, n2 = %u, total elements = %u", \
                            msg.n1, msg.n2, (msg.n1 + msg.n2));
    
    printf("First %zu bytes (hex): ", packed<64 ? packed : 64);
    for(size_t i=0; i<(packed<64 ? packed : 64); i++){
        if(i%16 == 0) printf("\n%04zu", i);
        printf("%02x ", buffer[i]);
    }
    printf("\n");


    
    free(msg.arr1);
    free(msg.arr2);
    return 0;
}