#include "simple-socket.h"

/*
Crée, initialise et enregistre auprès de l'OS un socket se connectant à un 
serveur TCP situé sur la machine (locale). 
*/
int create_client_local_tcp_socket(int server_port){
	// Adresse IP du serveur. Ici, notre propre machine.
	const char* server_name = "127.0.0.1";
	// Port du serveur sur lequel on désire se connecter.
	// Structure dans laquelle sera stockée l'adresse du serveur
	struct sockaddr_in server_address;
	
	// On remplace le contenu de la structure par des zéros afin d'éviter un
	// problème de pointeurs se trouvant dans cette structure.
	memset(&server_address, 0, sizeof(server_address));

	// L'adresse est une adresse de type IPv4
	server_address.sin_family = AF_INET;
	// A partir d'une chaine de caractère, remplir la structure contenant
	// l'adresse IP/
	inet_pton(AF_INET, server_name, &server_address.sin_addr);
	// htons = host to network short, ordonne l'entier 'short' correctement en
	// fonction de l'endianess de la machine, voir cours fctmnt. des ord.
	server_address.sin_port = htons(server_port);

	// On essaie de créer un socket, retourne -1 si cela échoue
	// SOCK_STREAM indique que l'on veut créer un socket TCP,
	// Pour UDP, il faut utiliser SOCK_DGRAM.
	int sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("could not create socket\n");
		return -1;
	}
	
    // On essaie de se connecter au serveur décrit dans server_address.
	// Si cela réussit, on pourra utiliser le socket "sock" pour envoyer/recevoir
	// des données.
	if (connect(sock, (struct sockaddr*)&server_address,
	            sizeof(server_address)) < 0) {
		printf("could not connect to server\n");
		return -1;
	}

    return sock;
}

/*
Crée, initialise et enregistre auprès de l'OS un socket se connectant à un 
serveur UDP situé sur la machine (locale). 
*/
int create_client_local_udp_socket(int server_port, struct sockaddr_in *server_address){
	// Adresse IP du serveur. Ici, notre propre machine.
	const char* server_name = "127.0.0.1";
	// On remplace le contenu de la structure par des zéros afin d'éviter un
	// problème de pointeurs se trouvant dans cette structure.
	memset(server_address, 0, sizeof(struct sockaddr_in));

	// L'adresse est une adresse de type IPv4
	server_address->sin_family = AF_INET;
	// A partir d'une chaine de caractère, remplir la structure contenant
	// l'adresse IP/
	inet_pton(AF_INET, server_name, &server_address->sin_addr);
	// htons = host to network short, ordonne l'entier 'short' correctement en
	// fonction de l'endianess de la machine, voir cours fctmnt. des ord.
	server_address->sin_port = htons(server_port);

	// On essaie de créer un socket, retourne -1 si cela échoue
	// SOCK_DGRAM indique que l'on veut créer des datagrammes et non un flux,
	// Pour UDP, il faut utiliser IPPROTO_UDP.
	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		printf("could not create socket\n");
		return -1;
	}

    // Pas de connexion avec UDP.

    return sock;
}

/*
Crée, initialise et enregistre auprès de l'OS un socket d'écoute pour une 
application jouant le rôle de serveur TCP.
*/
int create_listen_tcp_socket(int port){
	// Structure dans laquelle sera stockée l'adresse d'écoute du serveur
	struct sockaddr_in server_address;
	
	// On remplace le contenu de la structure par des zéros afin d'éviter un
	// problème de pointeurs se trouvant dans cette structure.
	memset(&server_address, 0, sizeof(server_address));

	// L'adresse est une adresse de type IPv4
	server_address.sin_family = AF_INET;
	// htons = host to network short, ordonne l'entier 'short' correctement en
	// fonction de l'endianess de la machine, voir cours fctmnt. des ord.
	server_address.sin_port = htons(port);
	// htonl = host to network long, pareil que htons mais pour les entiers 'long'
	// Ici, on veut que le serveur écoute sur toutes les adresses IP qui
	// appartiennent à la machine. En d'autres termes, lorsque l'on recoit un
	// segment TCP, si l'adresse de destination est l'une de nos adresse, on
	// veut accepter ce segment.
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	// On essaie de créer un socket, retourne -1 si cela échoue
	int listen_sock;
	if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("could not create listen socket\n");
		return -1;
	}

    // On specifie une option qui permet d'utiliser un port déjà utilisé.
    // Pratique quand on a oublié de 'close' un socket et que l'OS ne libère
    // pas tout de suite le port.
    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        printf("setsockopt(SO_REUSEADDR) failed\n");
        return -1;
    }

	// On lie la structure au serveur décrit dans la structure server_address
	if ((bind(listen_sock, (struct sockaddr *)&server_address,
	          sizeof(server_address))) < 0) {
		printf("could not bind socket\n");
		return -1;
	}

	// Nombre de clients que le serveur va accepter dans sa "file d'attente"
	int wait_size = 16;

	// On va initialiser l'écoute pour ce socket.
	if (listen(listen_sock, wait_size) < 0) {
		printf("could not open socket for listening\n");
		return -1;
	}
    return listen_sock;
}

/*
Crée, initialise et enregistre auprès de l'OS un socket d'écoute pour une 
application jouant le rôle de serveur TCP.
*/
int create_listen_udp_socket(int port){
	// Structure dans laquelle sera stockée l'adresse d'écoute du serveur
	struct sockaddr_in server_address;
	
	// On remplace le contenu de la structure par des zéros afin d'éviter un
	// problème de pointeurs se trouvant dans cette structure.
	memset(&server_address, 0, sizeof(server_address));

	// L'adresse est une adresse de type IPv4
	server_address.sin_family = AF_INET;
	// htons = host to network short, ordonne l'entier 'short' correctement en
	// fonction de l'endianess de la machine, voir cours fctmnt. des ord.
	server_address.sin_port = htons(port);
	// htonl = host to network long, pareil que htons mais pour les entiers 'long'
	// Ici, on veut que le serveur écoute sur toutes les adresses IP qui
	// appartiennent à la machine. En d'autres termes, lorsque l'on recoit un
	// segment TCP, si l'adresse de destination est l'une de nos adresses, on
	// veut accepter ce datagram.
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	// On essaie de créer un socket, retourne -1 si cela échoue
	int sock;
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		printf("could not create listen socket\n");
		return -1;
	}

    // On specifie une option qui permet d'utiliser un port déjà utilisé.
    // Pratique quand on a oublié de 'close' un socket et que l'OS ne libère
    // pas tout de suite le port.
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        printf("setsockopt(SO_REUSEADDR) failed\n");
        return -1;
    }

	// On lie la structure au serveur décrit dans la structure server_address
	if ((bind(sock, (struct sockaddr *)&server_address,
	          sizeof(server_address))) < 0) {
		printf("could not bind socket\n");
		return -1;
	}

    return sock;
}

