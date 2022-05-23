/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * client.c
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "helpers.h"

void run_client2(int sockfd) {
    char client_nr[2];
    memset(client_nr, 0, 1);

    char buf[BUFLEN];
    memset(buf, 0, BUFLEN);

    int byte_count = strlen(buf) + 1;

    int bytes_send;
    int bytes_remaining;
    int bytes_received;


    recv(sockfd, client_nr, BUFLEN, 0);
    printf("Client received number: %s\n", client_nr);

    while (1) {
        if (client_nr[0] == '1') {
            if (read(STDIN_FILENO, buf, BUFLEN - 1) > 0 && !isspace(buf[0])) {
                bytes_remaining = byte_count;

                // TODO a: Trimiteti lungimea mesajului
                int rc = send(sockfd, &byte_count, sizeof(int), 0);
                DIE(rc < 0, "Send error from Client 1\n");

                // TODO b: Trimiteti mesajul propriu-zis
                while (bytes_remaining > 0) {
                    bytes_send = send(sockfd, buf + (byte_count - bytes_remaining), bytes_remaining, 0);
                    DIE(bytes_send < 0, "Couldn't send to server!\n");
                    bytes_remaining -= bytes_send;
                }

                memset(buf, 0, BUFLEN);
            }

            client_nr[0] = '2';
        } else if (client_nr[0] == '2') {
                // TODO a: Receptionati lungimea mesajului
                int rc = recv(sockfd, &byte_count, sizeof(int), 0);
                DIE(rc < 0, "Receive error in Client 2\n");

                // TODO b: Receptionati mesajul propriu-zis
                while (bytes_remaining > 0) {
                    bytes_received = recv(sockfd, buf + (byte_count - bytes_remaining), bytes_remaining, 0);
                    DIE(bytes_received < 0, "Couldn't receive from client!");
                    bytes_remaining -= bytes_received;
                }

            fprintf(stderr, "Received from the other client: %s\n", buf);

            client_nr[0] = '1';
        }
    }

}

void run_client(int sockfd) {
    char buf[BUFLEN];
    memset(buf, 0, BUFLEN);

    int byte_count;

    while (read(STDIN_FILENO, buf, BUFLEN - 1) > 0 && !isspace(buf[0])) {
        byte_count = strlen(buf) + 1;

        int bytes_send;
        int bytes_remaining = byte_count;
        int bytes_received;

        
        // TODO 4: Trimiteti mesajul catre server prin sockfd
        bytes_send = send(sockfd, buf, BUFLEN, 0);
        DIE(bytes_send < 0, "Send failed\n");

        while (bytes_remaining > 0) {
            bytes_send = send(sockfd, buf + (byte_count - bytes_remaining), bytes_remaining, 0);
            DIE(bytes_send < 0, "Couldn't send to server!\n");
            bytes_remaining -= bytes_send;
        }

        memset(buf, 0, BUFLEN);
       
        // TODO 5: Receptionati un mesaj venit de la server
        bytes_received = recv(sockfd, buf, BUFLEN, 0);
        DIE(bytes_received < 0, "Receive from server error\n");

        fprintf(stderr, "Received: %s", buf);

        memset(buf, 0, BUFLEN);
    }
}

int main(int argc, char* argv[])
{
    int sockfd = -1;
    struct sockaddr_in serv_addr;
    socklen_t socket_len = sizeof(struct sockaddr_in);
    memset(&serv_addr, 0, socket_len);

    if (argc != 3) {
        printf("\n Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // TODO 1: Creati un socket TCP pentru conectarea la server
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd < 0, "Socket initialization failed\n");

    // TODO 2: Completati in serv_addr adresa serverului, familia de adrese si portul pentru conectare
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1234);

    // TODO 3: Creati conexiunea catre server
    int rs = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    DIE(rs < 0, "Connect client to server error\n");

    run_client2(sockfd);

    // TODO 6: Inchideti conexiunea si socketul creat
    close(rs);
    close(sockfd);

    return 0;
}
