#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
// utilisé pour gethostbyname
#include <netdb.h>

int create_client_local_tcp_socket();
int create_listen_tcp_socket();
