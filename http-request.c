
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
// utilisé pour gethostbyname
#include <netdb.h>

#include "http-parser.h"

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

int perform_request(int sock, int is_keepalive){
    char* request;
    if(is_keepalive)
        request = "GET / HTTP/1.1\r\nConnection: keep-alive\r\nHost: perdu.com\r\n\r\n";
    else
        request = "GET / HTTP/1.1\r\nConnection: close\r\nHost: perdu.com\r\n\r\n";

	// Zone mémoire qui contiendra des morceaux du corps de la réponse envoyée
    // par le serveur.
    char buffer[1000];
    // structure définie dans 'http-parser.h' que représente un champ avec son
    // nom et sa valeur.
    struct http_field field;

	// Contiendra le nombre d'octets reçus par le client (i.e. notre programme)
	int recv_len;
    // Contiendra la taille du corps de la réponse, lue dans l'entête.
    int content_length=-1;

    // On envoie la requête
    if( send(sock, request, strlen(request), 0) < 0)
    {
        printf("Failed to send\n");
        return -1;
    }
    // DEBUT DE LA LECTURE DE L'ENTETE
    // Lecture de la 1ere ligne de la reponse qui contient le status
    recv_len = read_line(sock, buffer, sizeof(buffer));

    // On lit chaque ligne/champ de l'entête.
    while((recv_len = read_line(sock, buffer, sizeof(buffer))) > 2){
        memset(&field, 0, sizeof(field));
        parse_field(buffer, recv_len, &field);
        // Si le champ que l'on vient de lire représente la taille du corps
        if(strcmp(field.name, "Content-Length") == 0){
            content_length = atoi(field.value);
        }
        
    }
    // FIN DE LA LECTURE DE L'ENTETE
    
    // Si on n'a pas trouvé le champ contenant la taille, on quitte.
    if(content_length == -1){
        return -1;
    }

    // DEBUT DE LA LECTURE DU CORPS
    printf("content length:%d\n", content_length);
    // On a fini de lire l'entête, on peut maintenant lire le corps de la
    // réponse dont on connait la taille.
    while(content_length > 0){
        memset(buffer, 0, sizeof(buffer));
        // On demande à lire le corps complètement mais il est possible
        // que l'on recevra moins que cette quantité demandée
        recv_len = recv(sock, buffer, content_length, 0);
        if(recv_len < 0){
            return -1;
        }
        // on affiche les 'recv_len' octets reçus.
        printf("%s", buffer);
        // Mise à jour de ce qu'il reste à lire dans le socket.
        content_length -= recv_len;
    }
    printf("\n");
    // FIN DE LA LECTURE DU CORPS
    return 0;
}

int main() {
    // Première requête
    int sock = create_socket();
    if(sock < 0){
        return 1;
    }
    if(perform_request(sock,1) < 0){
        return 1;
    }
    if(perform_request(sock,0) < 0){
        return 1;
    }
	
    close(sock);
    return 0;
}
