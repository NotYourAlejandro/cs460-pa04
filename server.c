#include "server.h"

void* handle_client(void* targs);

void pexit(char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    // prep socket
    int server_socket;
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        pexit("Error creating socket");

    // assemble addr to bind socket to
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(SERVER_PORT),
    };

    // bind socket to addr
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
        pexit("Failed to bind to socket");

    printf("Listening from %s on port %d\n", SERVER_ADDR, SERVER_PORT);

    // begin main server loop
    while (TRUE)
    {
        // accept connection to client
        struct sockaddr_in * client_addr = malloc(sizeof(struct sockaddr_in));
        socklen_t * client_addr_len = malloc(sizeof(socklen_t));
        *client_addr_len = sizeof(struct sockaddr_in);

        if (recvfrom(server_socket, NULL, 0, 0, (struct sockaddr *)client_addr, client_addr_len) < 0)
            fprintf(stderr, "Failed to receive data from client\n");
 
        // assemble thread args
        struct thread_args *targs = malloc(sizeof(struct thread_args));
        targs->client_addr   = client_addr;
        targs->client_addr_len = client_addr_len;
        targs->server_socket = server_socket;
 
        // send connection off to new thread
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, (void*)targs) != 0)
            pexit("Failed to create thread");

        // let thread handle its memory
        if (pthread_detach(thread) != 0)
            pexit("Error detaching thread");

    }
    close(server_socket);
}

// thread main called by pthread_create
void* handle_client(void* targs)
{
    // unpack thread args
    struct thread_args * args = (struct thread_args *) targs;
    struct sockaddr_in * client_addr = args->client_addr;
    int server_socket = args->server_socket;
    socklen_t * client_addr_len = args->client_addr_len;

    // setup relevant vars
    char time_buffer[BUFFER_SIZE];
    time_t unix_seconds = time(NULL);
    struct tm* current_time = gmtime(&unix_seconds);

    if (!client_addr) {
        fprintf(stderr, "No return client provided to thread!");
        
        free(client_addr);
        free(client_addr_len);
        free(args);
        pthread_exit(NULL);
    }

    // assemble daytime string and send to client
    strftime(time_buffer, sizeof(time_buffer), "%y-%m-%d %H:%M:%S UTC*\n", current_time);
    printf("UDP packet received, sending back the time...\n");
    if (sendto(server_socket, time_buffer, strlen(time_buffer), 0, (struct sockaddr *) client_addr, *client_addr_len) == -1)
        perror("Error when sending data to client");

    free(client_addr);
    free(client_addr_len);
    free(args);
    pthread_exit(NULL);
}
