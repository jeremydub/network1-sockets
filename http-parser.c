#include "http-parser.h"
#include <sys/socket.h>
#include <string.h>

size_t
read_line(int fd, char *buf, size_t max_len)
{
    // Nombre d'octets reçus lors d'un recv()
    size_t recv_len;
    // Nombre d'octets reçus jusq'à présent
    size_t current_size;
    // Contiendra le caractère reçu dans le socket.
    char ch;

    current_size = 0;
    while(1) {
        recv_len = recv(fd, &ch, 1,0);

        // s'il y a une erreur
        if (recv_len == -1) {
            return -1;              
        } else if (recv_len == 0) {      /* End of File, i.e. fin du flux tcp */
            // si on a lu aucun octet au total
            if (current_size == 0)
                return 0;
            // Sinon, on a lu au moins un octet, il faut sortir de la boucle
            // pour ajouter le caractère de fin de string, '\0'.
            else
                break;

        } else { /* On est dans le cas où on a reçu un caractère */
            // On ajoute au buffer seulement s'il n'y a pas déjà 'max_len'
            // caractères à l'interieur, sinon on igore ces derniers caractères.
            if (current_size < max_len - 1) {
                current_size++;
                *buf++ = ch;
            }
            // On a trouvé un caractère de fin de ligne, on peut sortir de la
            // boucle.
            if (ch == '\n')
                break;
        }
    }

    *buf = '\0';
    return current_size;
}

size_t
parse_field(char *line, size_t len, struct http_field *field)
{
    // On cherche la position du caractère ':' séparant le nom de la valeur.
    int pos = 0;
    while (line[pos] && line[pos++] != ':');

    strncpy(field->name, line, pos-1);
    // 'line+pos+1' car on ne veut pas de l'espace après ':',
    // 'len-pos-3' car on ignore '\r\n' en fin de ligne.
    strncpy(field->value, line+pos+1, len-pos-3);
}
