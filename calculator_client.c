#include "simple-socket.h"

int main() {
    int sock = create_client_local_tcp_socket(8888);

    if(sock < 0){
        printf("Could not create/connect socket\n");
        return 1;
    }

	// On crée notre buffer d'envoi qui contiendra exactement 9 octets
	// (4+4+1, voir énoncé)
	// buffer est également un pointeur vers le premier 'char' du tableau
	char buffer[9];

	// Contiendra le nombre d'octet reçus par le client
	int recv_n;

	// Variable qui sera utilisée pour stocker le résultat envoyé par le serveur
	int32_t resultat = 0;

	// A chaque passage dans la boucle, on va demander d'entrer 2 nombres
	// et un operateur.
	while(1)
    {
        printf("Entrez un premier nombre: ");
        // On récupère l'entier de 32 bits que l'on stocke sur 4 octets à partir
        // de l'adresse décrite par le pointeur 'buffer'
        scanf("%ld" , (int32_t*)buffer);

        printf("Entrez un second nombre: ");
        scanf("%ld", (int32_t*)(buffer+4));

        printf("Entrez un operateur parmi +,-,*,/ : ");
        scanf(" %c" , (buffer+8));

        // On envoie les 9 premiers octets à partir de l'adresse décrite par le
        // pointeur 'buffer'
        if( send(sock, buffer, 9, 0) < 0)
        {
            printf("Failed to send\n");
            return 1;
        }

        // On lit le résultat du serveur qui est stocké dans un entier de 32
        // bits, i.e. 4 octets
        if( (recv_n = recv(sock, &resultat , 4, 0)) < 0)
        {
            printf("Failed to receive\n");
            break;
        }
    		
    	printf("Resultat: %u\n\n", resultat);
		
    }
	
    close(sock);
    return 0;
}
