#include "simple-socket.h"

/*
Le serveur envoie, sans attendre mais en plusieurs appels à send, 'step' caractères d'un message prédéfini au client actuel.
*/
int handle_client(int client_sock, struct sockaddr_in* client_address){
	const char* MESSAGE = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int len = strlen(MESSAGE);
    int step = 7;
    int i=0;
    while(i<len){
        if(i+step > len)
            step -= (i+step)%len;
        send(client_sock, MESSAGE+i, step, 0);
        printf("Sent %d bytes\n", step);
        i += step;
    }
}

int main(int argc, char *argv[]) {
    int listen_sock = create_listen_tcp_socket(8888);

	// Structure utilisée pour stocker l'adresse du client qui se connectera au serveur
	struct sockaddr_in client_address;
	int client_address_len;

	// Chaque passage de la boucle représente la gestion d'un client
	while (1) {
		// On crée un socket lié cette fois-ci à la connexion avec le client en
		// particulier. Cet appel est bloquant. Tant que personne ne se connecte
		// au serveur, l'exécution sera suspendue.
		int sock = accept(listen_sock, (struct sockaddr *)&client_address,
                          &client_address_len);
		if (sock < 0) {
			printf("could not open a socket to accept data\n");
			return 1;
		}
        handle_client(sock, &client_address);
        // On ferme le socket lié au client
		close(sock);
	}

	// On ferme le socket d'écoute sur notre machine
	close(listen_sock);
	return 0;
}
