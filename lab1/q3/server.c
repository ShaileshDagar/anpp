// udp_server.c
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>   // for errno and EINTR

#define PORT 12345
#define BUF_SIZE 1024

volatile sig_atomic_t stop = 0;

void handle_sigint(int sig) {
    stop = 1;  // mark flag instead of calling unsafe functions
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buffer[BUF_SIZE];

    // signal(SIGINT, handle_sigint);
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening on port %d...\n", PORT);

    while (!stop) {
        int nbytes = recvfrom(sockfd, buffer, BUF_SIZE, 0,
                              (struct sockaddr *)&cliaddr, &len);
        if (nbytes < 0) {
            if (errno == EINTR) {
                printf("Program Interrupted by SIGINT\n");
                break;
            }
            perror("recvfrom failed");
            continue;
        }

        int n;
        memcpy(&n, buffer, sizeof(int));
        n = ntohl(n); // convert to host order

        if (n <= 0 || n > 100) {
            printf("Invalid size received. Ignoring...\n");
            continue;
        }

        int *numbers = (int *)(buffer + sizeof(int));
        int valid = 1;

        // Check validity: only even integers
        for (int i = 0; i < 2 * n; i++) {
            int val = ntohl(numbers[i]);
            if (val % 2 != 0) {
                valid = 0;
                break;
            }
        }

        if (!valid) {
            printf("Invalid data (odd number). Ignoring...\n");
            continue; // no reply
        }

        // Compute sums
        int sum1 = 0, sum2 = 0;
        for (int i = 0; i < n; i++) {
            sum1 += ntohl(numbers[i]);
            sum2 += ntohl(numbers[n + i]);
        }

        int reply[2];
        reply[0] = htonl(sum1);
        reply[1] = htonl(sum2);

        sendto(sockfd, reply, sizeof(reply), 0,
               (struct sockaddr *)&cliaddr, len);

        printf("Processed valid data from client. Reply sent: [%d, %d]\n",
               sum1, sum2);
    }

    close(sockfd);
    return 0;
}