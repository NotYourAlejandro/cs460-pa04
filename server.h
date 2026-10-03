#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT "7777"
#define NUM_CONNECTIONS 2

#define BUF_SIZE 80

#define TRUE 1
#define FALSE 0

