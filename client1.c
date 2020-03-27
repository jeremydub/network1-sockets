#include "simple-socket.h"

/*
Client se connectant à un serveur TCP se trouvant sur la machine (port 8888).
Une fois connecté, il affiche les caractères envoyé par le serveur jusqu'au 
moment où ce dernier coupe la connexion.
*/
int main() {
    int sock = create_client_local_tcp_socket(8888);

    if(sock < 0){
        printf("Could not create/connect socket\n");
        return 1;
    }

	char buffer[10];
    // On remplit le buffer de 0.
    memset(buffer, 0, 10);

	int recv_len;
    // Tant que l'on reçoit des caractères
    while((recv_len = recv(sock, buffer , 10, 0)) > 0){
        printf("received %d bytes : '%s'\n",recv_len, buffer);
        memset(buffer, 0, 10);
	}
    // Une fois que recv_len <= 0, la connexion est terminée/interrompue.
    close(sock);
    return 0;
}
