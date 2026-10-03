#include "server.h"


int main(int argc, char *argv[]) {
    struct addrinfo hint, *result, *tmp;
    struct sockaddr_in *addr;
    char addr_str[INET_ADDRSTRLEN];
    char buffer[BUF_SIZE];
    char c = 0;
    int sock_fd;
  
    bzero(&hint, sizeof(struct addrinfo));
  
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_DGRAM;
  
    if (getaddrinfo(SERVER_ADDR, SERVER_PORT, &hint, &result)) {
        fprintf(stderr, "Failed to get address info.\n");
        return EXIT_FAILURE;
    }

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Failed to create socket\n");
        return EXIT_FAILURE;
    }
  
    if (connect(sock_fd, result->ai_addr, result->ai_addrlen)) {
        fprintf(stderr, "Failed to connect to server\n");
        return EXIT_FAILURE;
    }
  
    while (c != '*') {
        // read(sock_fd, &c, sizeof(char));
        recvfrom(sock_fd, &buffer, BUF_SIZE, 0, hint.ai_addr, &hint.ai_addrlen);

        puts(buffer);
    }
    printf("\n");
 
    close(sock_fd);
    freeaddrinfo(result);
  
    return EXIT_SUCCESS;
}
