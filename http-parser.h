#include <stddef.h>
#include <errno.h>

struct http_field{
    char name[50];
    char value[200];
};

/*

*/
size_t read_line(int fd, char *buf, size_t max_len);

/*

*/
size_t parse_field(char *line, size_t len, struct http_field *field);
