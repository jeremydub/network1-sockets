#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
// utilisé pour gethostbyname
#include <netdb.h>

int create_client_local_tcp_socket(int server_port);
int create_listen_tcp_socket(int port);
int create_client_local_udp_socket(int server_port, struct sockaddr_in *server_address);
int create_listen_udp_socket(int port);
