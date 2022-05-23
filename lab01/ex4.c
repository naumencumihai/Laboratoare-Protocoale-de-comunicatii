#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define MAX 100

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    } else {
        FILE * fp;
        fp = fopen(argv[1], "r");
        char* line;
        line = (char*)malloc(MAX*sizeof(char));
        long unsigned int len = 0;

        while(getline(&line, &len, fp) > 0) {

            printf("%s", line);
        }

        free(line);
        fclose(fp);
    }

    return(0);
}