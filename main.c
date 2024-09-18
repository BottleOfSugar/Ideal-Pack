#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>      // Dodaj ten nagłówek
#include <errno.h>      // Dodaj ten nagłówek

#define TIMEOUT 1  // Timeout in seconds

// Function to check if a port is open
int is_port_open(const char *host, int port) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct timeval tv;
    fd_set fdset;
    int result;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 0;
    }

    // Set timeout
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);

    // Set non-blocking mode
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    // Attempt to connect
    result = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (result < 0 && errno != EINPROGRESS) {
        close(sockfd);
        return 0;
    }

    // Use select to wait for the connection to complete
    FD_ZERO(&fdset);
    FD_SET(sockfd, &fdset);
    result = select(sockfd + 1, NULL, &fdset, NULL, &tv);

    if (result > 0 && FD_ISSET(sockfd, &fdset)) {
        close(sockfd);
        return 1; // Port is open
    }

    close(sockfd);
    return 0; // Port is closed
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <hostname> <port1,port2,...>\n", argv[0]);
        return 1;
    }

    char *host = argv[1];
    char *ports_str = argv[2];
    char *token;
    int port;

    // Tokenize the ports string and check each port
    token = strtok(ports_str, ",");
    while (token != NULL) {
        port = atoi(token);
        if (is_port_open(host, port)) {
            printf("Port %d is open\n", port);
        } else {
            printf("Port %d is closed\n", port);
        }
        token = strtok(NULL, ",");
    }

    return 0;
}
