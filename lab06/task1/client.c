#include "utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 1024
#define CHUNKSIZE 1024
#define SENT_FILENAME "file.bin"

void send_file(int sockfd, struct sockaddr_in server_address, char *filename) {
  int fd = open(filename, O_RDONLY);
  DIE(fd < 0, "open");

  while (1) {
    char buffer[CHUNKSIZE];
    /* TODO B.1: Read a chuck from the file */
    int n = read(fd, buffer, sizeof(buffer));
    //tratam eroare
    DIE(n < 0, "read");

    if (n == 0) break;

    /* TODO B.2: Send the current chuck to the server */
    int m = sendto(sockfd, buffer, n, 0, (struct sockaddr*)&server_address, sizeof(server_address));    
    DIE(m < 0, "sendto");

    /* TODO B.3: Make sure that the server can identify the end of the file */
  }
  int rc = sendto(sockfd, NULL, 0, 0, (struct sockaddr*)&server_address, sizeof(server_address));
  DIE(rc < 0, "send to");
}

void send_and_receive_a_message(int sockfd, struct sockaddr_in server_address) {
  char *hello = "Hello from client";
  char buffer[MAXLINE];

  // The client sends the first message
  int n =
      sendto(sockfd, (const char *)hello, strlen(hello), 0,
             (const struct sockaddr *)&server_address, sizeof(server_address));
  DIE(n < 0, "sendto");
  printf("%d bytes sent.\n", n);

  /*socketlen_t is an integer type specific for the length of
   addresses stored in struct sockaddr. Before "receiving" an address from
  a function, the value should be set to the amount sizeof(addr_var)

  recvfrom() will set the value to the size of the output address
  */
  struct sockaddr_in sender_address;
  socklen_t sender_address_len = sizeof(sender_address);

  n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
               (struct sockaddr *)&sender_address, &sender_address_len);

  if (n < 0) {
    perror("recvfrom");
    exit(EXIT_FAILURE);
  } else {
    // Add \0 to the end of the received message
    buffer[n] = '\0';

    // Print the received message
    printf("Received %d bytes from %s: %s\n", n,
           inet_ntoa(sender_address.sin_addr), buffer);
  }
}

int main(int argc, char *argv[]) {
  struct sockaddr_in servaddr;

  if (argc < 3) {
    fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
    fprintf(stderr, "Eg: %s 1.2.3.4 10001\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Creating socket file descriptor
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  DIE(sockfd < 0, "socket");

  // Filling server information
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  int rc = inet_aton(argv[1], &servaddr.sin_addr);
  if (rc == 0) {
    fprintf(stderr, "ERROR: %s is not a valid IP address.\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  /* TODO A: Read the above and test if you can send and receive a message. */
  //send_and_receive_a_message(sockfd, servaddr);

  /* TODO B: Implement the function to send a file.
      The line above should be comented when solving this TODO.
  */

  send_file(sockfd, servaddr, SENT_FILENAME);

  // Close the socket
  close(sockfd);
  return 0;
}
