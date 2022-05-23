/*
 * Protocoale de comunicatii
 * Laborator 11 - E-mail
 * send_mail.c
 */

#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>  
#include <unistd.h>     
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define SMTP_PORT 25
#define MAXLEN 500

// From lab 8
#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

/**
 * Citeste maxim maxlen octeti de pe socket-ul sockfd in
 * buffer-ul vptr. Intoarce numarul de octeti cititi.
 */
ssize_t read_line(int sockd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *buffer;

    buffer = vptr;

    for (n = 1; n < maxlen; n++) {
        if ((rc = read(sockd, &c, 1)) == 1) {
            *buffer++ = c;

            if (c == '\n') {
                break;
            }
        } else if (rc == 0) {
            if (n == 1) {
                return 0;
            } else {
                break;
            }
        } else {
            if (errno == EINTR) {
                continue;
            }

            return -1;
        }
    }

    *buffer = 0;
    return n;
}

/**
 * Trimite o comanda SMTP si asteapta raspuns de la server. Comanda
 * trebuie sa fie in buffer-ul sendbuf. Sirul expected contine
 * inceputul raspunsului pe care trebuie sa-l trimita serverul
 * in caz de succes (de exemplu, codul 250). Daca raspunsul
 * semnaleaza o eroare, se iese din program.
 */
void send_command(int sockfd, const char sendbuf[], char *expected)
{
    char recvbuf[MAXLEN];
    int nbytes;
    char CRLF[2] = {13, 10};

    printf("Trimit: %s\n", sendbuf);
    write(sockfd, sendbuf, strlen(sendbuf));
    write(sockfd, CRLF, sizeof(CRLF));

    nbytes = read_line(sockfd, recvbuf, MAXLEN - 1);
    recvbuf[nbytes] = 0;
    printf("Am primit: %s", recvbuf);

    if (strstr(recvbuf, expected) != recvbuf) {
        printf("Am primit mesaj de eroare de la server!\n");
        exit(-1);
    }
}

int main(int argc, char **argv) {
    int ret;
    int sockfd;
    //int port = SMTP_PORT;
    struct sockaddr_in servaddr;
    char server_ip[INET_ADDRSTRLEN];
    char sendbuf[MAXLEN]; 
    char recvbuf[MAXLEN];
    char filename[MAXLEN];
    strcpy(filename, argv[2]);

    if (argc != 3) {
        printf("Utilizare: ./send_msg adresa_server nume_fisier\n");
        exit(-1);
    }

    strncpy(server_ip, argv[1], INET_ADDRSTRLEN);

    // TODO 1: creati socket-ul TCP client
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd < 0, "[ERROR] Creating socket failed\n");

    // TODO 2: completati structura sockaddr_in cu
    // portul si adresa IP ale serverului SMTP
    //memset(&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(2000);
    ret = inet_aton(argv[1], &servaddr.sin_addr);
    DIE(ret < 0, "[ERROR] inet_aton() failed\n");
        
    // TODO 3: conectati-va la server
    ret = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));
    DIE(ret < 0, "[ERROR] Connecting to server failed\n");

    // se primeste mesajul de conectare de la server
    read_line(sockfd, recvbuf, MAXLEN -1);
    printf("Am primit: %s\n", recvbuf);

    // se trimite comanda de HELO
    sprintf(sendbuf, "HELO localhost");
    send_command(sockfd, sendbuf, "250");

    // TODO 4: trimiteti comanda de MAIL FROM
    sprintf(sendbuf, "MAIL FROM: <profesor@gmail.com>");
    send_command(sockfd, sendbuf, "250");

    // TODO 5: trimiteti comanda de RCPT TO
    sprintf(sendbuf, "RCPT TO: <naumencumihai@gmail.com>");
    send_command(sockfd, sendbuf, "250");

    // TODO 6: trimiteti comanda de DATA
    sprintf(sendbuf, "DATA");
    send_command(sockfd, sendbuf, "250");

    // TODO 7: trimiteti e-mail-ul (antete + corp + atasament)
    memset(sendbuf, 0, MAXLEN);
    strcat(sendbuf, "MIME-Version: 1.0\r\n");
    strcat(sendbuf, "From: Profesor <profesor@gmail.com>\r\n");    
    strcat(sendbuf, "To: Naumencu Mihai <naumencumihai@gmail.com>\r\n");
    strcat(sendbuf, "Subject: Laborator PCom\r\n");
    strcat(sendbuf, "Content-Type: multipart/mixed; boundary=xxx\r\n");
    strcat(sendbuf, "\r\n--xxx\r\n");
    strcat(sendbuf, "Content-Type: text/plain\r\n\r\n");
    strcat(sendbuf, "This is the body of the e-mail.\r\nBest regards\r\n\r\n");
    strcat(sendbuf, "--xxx\r\n");
    strcat(sendbuf, "Content-Type: text/plain\r\n");
    strcat(sendbuf, "Content-Disposition: attachment; filename=\"");
    strcat(sendbuf, filename);
    strcat(sendbuf, "\"\r\n\r\n");

    // Read from file
    FILE *fp = fopen("file.txt", "r");
    char filebuffer[MAXLEN];
    fgets(filebuffer, MAXLEN, fp);
    fclose(fp);

    // Write file content to send buffer
    strcat(sendbuf, filebuffer);

    send_command(sockfd, sendbuf, "250");

    // TODO 8: trimiteti comanda de QUIT
    memset(sendbuf, 0, MAXLEN);
    sprintf(sendbuf, "QUIT");
    send_command(sockfd, sendbuf, "250");

    // TODO 9: inchideti socket-ul TCP client
    close(sockfd);

    return 0;
}
