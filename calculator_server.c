#include "simple-socket.h"

int handle_client(int client_sock, struct sockaddr_in* client_address){
    // On crée notre buffer de réception qui contiendra exactement 9 octets
    // (4+4+1, voir énoncé)
    // buffer est également un pointeur vers le premier 'char' du tableau
    char buffer[9];

    // Contiendra le nombre d'octet reçus par le serveur
    int recv_len;

    // On crée des variables qui vont faciliter l'utilisation des données reçues
    int32_t nombre1, nombre2;
    char operateur;

    // Variable qui sera utilisée pour stocker le résultat
    int32_t resultat = 0;

    // On affiche l'adresse IP du client, parce que .. pourquoi pas ;)
    printf("Adresse IP du client: %s\n",
           inet_ntoa(client_address->sin_addr));

    // On va répondre aux requêtes tant que la connexion n'est pas fermée.
    // On lit jusqu'à 9 octets qui ont été envoyés par le client. Tant que le client
    // n'a rien envoyé, l'appel de la fonction est bloquant. On s'attend à ce que
    // le client envoie 9 octets en un seul appel de 'send'.
    while ((recv_len = recv(client_sock, buffer, 9, 0)) > 0) {

        // Si on n'a pas reçu exactement 9 octets, on met fin à la discussion avec
        // le client.
        if(recv_len != 9){
            break;
        }

        // On écrase le contenu de 'nombre1' par les 4 octets qui se trouve à
        // l'adresse 'buffer' jusque buffer+3 (buffer représente une adresse)
        memcpy(&nombre1, buffer, 4);
        // On écrase le contenu de 'nombre2' par les 4 octets qui se trouve à
        // l'adresse 'buffer+4' jusque 'buffer+7'
        memcpy(&nombre2, buffer+4, 4);
        
        // On récupère l'opérateur
        operateur=buffer[8];

        // On gère le calcul en fonction de l'opérateur
        if(operateur == '+')
            resultat = nombre1 + nombre2;
        else if(operateur == '-')
            resultat = nombre1 - nombre2;
        else if(operateur == '*')
            resultat = nombre1 * nombre2;
        else if(operateur == '/')
            resultat = nombre1 / nombre2;

        // on envoie le résultat
        send(client_sock, &resultat, 4, 0);
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
