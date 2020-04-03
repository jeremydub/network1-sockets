#include "simple-socket.h"

int handle_message(int sock){
    // On crée notre buffer de réception qui contiendra exactement 9 octets
    // (4+4+1, voir énoncé)
    // buffer est également un pointeur vers le premier 'char' du tableau
    char buffer[9];

    // Contiendra les informations relatives au client qui a envoyé un message
    struct sockaddr_in client;

    // Contiendra le nombre d'octet reçus par le serveur
    int recv_len;
    // Contiendra la taille de la structure contenant l'adresse du server
    int slen;

    // On crée des variables qui vont faciliter l'utilisation des données reçues
    int32_t nombre1, nombre2;
    char operateur;

    // Variable qui sera utilisée pour stocker le résultat
    int32_t resultat = 0;

    // On lit jusqu'à 9 octets qui ont été envoyés par le client. Tant que le client
    // n'a rien envoyé, l'appel de la fonction est bloquant. On s'attend à ce que
    // le client envoie 9 octets en un seul appel de 'send'.
    recv_len = recvfrom(sock, buffer, 9, 0, (struct sockaddr *) &client, &slen);

    // Si on n'a pas reçu exactement 9 octets, on met fin à la discussion avec
    // le client.
    if(recv_len != 9){
        return -1;
    }

    // On affiche l'adresse IP du client, parce que .. pourquoi pas ;)
    printf("Adresse IP du client: %s\n",
           inet_ntoa(client.sin_addr));

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
    sendto(sock, &resultat, 4, 0, (struct sockaddr *) &client, slen);
}

int main(int argc, char *argv[]) {
    int sock = create_listen_udp_socket(7777);

	// Structure utilisée pour stocker l'adresse du client qui envoie un message.
	struct sockaddr_in client_address;
	int client_address_len;

	// Chaque passage de la boucle représente la gestion d'un client
	while (1) {
        handle_message(sock);
	}

	// On ferme le socket d'écoute sur notre machine
	close(sock);
	return 0;
}
