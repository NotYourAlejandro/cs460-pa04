#include "server.h"

void pexit(char* msg) {
    fprintf(stderr, msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    struct addrinfo *getaddr_res, *getaddr_tmp;
    struct addrinfo tx_addrinfo = { .ai_family = AF_INET, .ai_socktype = SOCK_DGRAM };
    struct sockaddr_in rx_addrinfo;
    socklen_t rx_addrinfo_len = sizeof(rx_addrinfo);

    char addr_str[INET_ADDRSTRLEN];
    char buffer[BUFFER_SIZE];
    char buffer_char = 0;
    char port[5];
    int sock_fd;
  
    // prep basic variables
    memset(&tx_addrinfo, 0, sizeof(tx_addrinfo));

    // convert port from into to string
    sprintf(port, "%d", SERVER_PORT);

    // set up socket
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        pexit("Failed to create socket\n");
  
    // resolve address
    if (getaddrinfo(SERVER_ADDR, port, &tx_addrinfo, &getaddr_res) != 0)
        pexit("Failed to get address info.\n");

    // iter through resolution linked list and find one that successfully connects
    for (getaddr_tmp = getaddr_res; getaddr_tmp; getaddr_tmp = getaddr_tmp->ai_next)
        if (connect(sock_fd, getaddr_tmp->ai_addr, getaddr_tmp->ai_addrlen) == 0)
            break;
  
    if (!getaddr_tmp)
        pexit("Failed to resolve address\n");
  
    // send the empty packet to the server
    if (sendto(sock_fd, NULL, 0, 0, getaddr_tmp->ai_addr, getaddr_tmp->ai_addrlen) == -1)
        pexit("Failed to send empty packet to server\n");

    // wait for server's response, then print it out
    while (!strrchr(buffer, '*')) {
        if (recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&rx_addrinfo, &rx_addrinfo_len) == -1) {
            perror("Failed to receive a packet from the server");
            break;
        }

        // unpack addresses from both rx and tx address info
        struct sockaddr_in *tx_addr = (struct sockaddr_in *) getaddr_tmp->ai_addr;
        struct sockaddr_in *rx_addr = &rx_addrinfo;
       
        if (tx_addr->sin_family == rx_addr->sin_family &&
            tx_addr->sin_port == rx_addr->sin_port &&
            tx_addr->sin_addr.s_addr == rx_addr->sin_addr.s_addr)
            printf("%s", buffer);
    }
 
    // clean up
    close(sock_fd);
    freeaddrinfo(getaddr_res);
  
    return EXIT_SUCCESS;
}
