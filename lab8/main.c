#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <crypt.h>

int main(int argc, char *argv[]) {
 if (argc != 3) {
        fprintf(stderr, "HELP %s <password> <salt>\n", argv[0]);
        return EXIT_FAILURE; }
char *password = argv[1];
 char *salt = argv[2];
char salt_format[256];
snprintf(salt_format, sizeof(salt_format), "$6$%s$", salt);
char *hash = crypt(password, salt_format);

    if (hash == NULL) {
        perror("crypt");
        return EXIT_FAILURE;}
printf("%s\n", hash);
    return EXIT_SUCCESS;}

