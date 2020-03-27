
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
// utilisé pour gethostbyname
#include <netdb.h>

int create_socket(){
	const char* server_name = "perdu.com";
	const int server_port = 80;
	struct sockaddr_in server_address;
    // contiendra les informations (IP,..) d'un nom de domaine
    struct hostent *he;

	memset(&server_address, 0, sizeof(server_address));

    // Résolution du nom de domaine
    if ( (he = gethostbyname(server_name) ) == NULL ) {
        return 1; /* error */
    }

	server_address.sin_family = AF_INET;
    // On copie l'adresse IP obtenue après résolution DNS
    memcpy(&server_address.sin_addr, he->h_addr_list[0], he->h_length);
	server_address.sin_port = htons(server_port);

	int sock;
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
	    printf("could not create socket\n");
	    return -1;
	}

	if (connect(sock, (struct sockaddr*)&server_address,
	            sizeof(server_address)) < 0) {
	    printf("could not connect to server\n");
	    return -1;
	}
    return sock;
}

int perform_request_close(int sock){
	const char* request = "GET / HTTP/1.1\r\nConnection: close\r\nHost: perdu.com\r\n\r\n";
	// Zone mémoire qui contiendra la réponse envoyée par le serveur. On
    // Suppose que celle-ci aura une taille inférieure à 1000 octets.
    char buffer[1000];
	memset(buffer, 0, sizeof(buffer));

	// Contiendra le nombre d'octets reçus par le client (i.e. notre programme)
	int recv_n;

    // On envoie la requête
    if( send(sock, request, strlen(request), 0) < 0)
    {
        printf("Failed to send\n");
        return -1;
    }

    // On lit la réponse du serveur HTTP. On ne connait pas la taille de celle-ci
    // mais étant donné que l'on utilise une requête de type 'close', le serveur
    // fermera la connexion TCP, on peut donc naïvement lire une taille
    // arbitrairement grande.
    recv_n = recv(sock, &buffer , sizeof(buffer), 0);
    if( recv_n < 0)
    {
        printf("Failed to receive\n");
        return -1;
    }
    printf("%s\n", buffer);
    return 0;
}

int main() {
    // Première requête
    int sock = create_socket();
    if(sock < 0){
        return 1;
    }
    if(perform_request_close(sock) < 0){
        return 1;
    }
    close(sock);

    // Deuxième requête
    sock = create_socket();
    if(sock < 0){
        return 1;
    }

    if(perform_request_close(sock) < 0){
        return 1;
    }
	
    close(sock);
    return 0;
}
