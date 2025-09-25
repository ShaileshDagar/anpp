// udp_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

#define PORT 12345
#define SERVER_IP "127.0.0.1"
#define BUF_SIZE 1024
#define MAX_RETRIES 3

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    socklen_t len = sizeof(servaddr);

    // Arrays to send (only even numbers)
    int arr1[] = {2, 4, 6, 8};
    int arr2[] = {10, 12, 14, 16};
    int n = sizeof(arr1) / sizeof(arr1[0]);

    char buffer[BUF_SIZE];
    int offset = 0;

    // Pack: n + arr1 + arr2
    int net_n = htonl(n);
    memcpy(buffer + offset, &net_n, sizeof(int));
    offset += sizeof(int);

    for (int i = 0; i < n; i++) {
        int val = htonl(arr1[i]);
        memcpy(buffer + offset, &val, sizeof(int));
        offset += sizeof(int);
    }
    for (int i = 0; i < n; i++) {
        int val = htonl(arr2[i]);
        memcpy(buffer + offset, &val, sizeof(int));
        offset += sizeof(int);
    }

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr);

    // Set timeout for recvfrom
    struct timeval tv;
    tv.tv_sec = 3;   // 3 seconds
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    int attempt = 0;
    int reply[2];
    int received = 0;

    while (attempt < MAX_RETRIES && !received) {
        sendto(sockfd, buffer, offset, 0,
               (struct sockaddr *)&servaddr, len);
        printf("Sent arrays to server (attempt %d)\n", attempt + 1);

        int nbytes = recvfrom(sockfd, reply, sizeof(reply), 0,
                              (struct sockaddr *)&servaddr, &len);

        if (nbytes < 0) {
            perror("Timeout or error, retransmitting...");
            attempt++;
        } else {
            int sum1 = ntohl(reply[0]);
            int sum2 = ntohl(reply[1]);
            printf("Received sums from server: [%d, %d]\n", sum1, sum2);
            received = 1;
        }
    }

    if (!received) {
        printf("No reply after %d attempts. Giving up.\n", MAX_RETRIES);
    }

    close(sockfd);
    return 0;
}