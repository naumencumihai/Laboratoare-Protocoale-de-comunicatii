#include <unistd.h> /* pentru open(), exit() */
#include <fcntl.h> /* O_RDWR */
#include <stdio.h> /* perror() */
#include <errno.h>
#include <stdlib.h>

struct Packet {
    char  payload[100];
    int   sum;
    int   size;
}; 

int main() {
    struct Packet p;
    int fd = open("ex3.txt", O_RDONLY);
    
    while (1) {
        int rc = read(fd, &p, sizeof(p));
        if (rc > 0) {
            printf("Payload: %s", p.payload);
            printf("Sum: %d", p.sum); 
            printf("Size: %d", p.size);
        } else if (rc == 0) {
            break;
        } else {
            printf("Eroare");
            break;
        }
    }
}