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
#include <sys/uio.h>
#include <unistd.h>

#define MAXLINE 1024
#define CHUNKSIZE 1024
#define SAVED_FILENAME "received_file.bin"

void recv_a_file(int sockfd, char *filename) {
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  DIE(fd < 0, "open");

  while (1) {
    char buffer[CHUNKSIZE];
    /* TODO B.1: Receive a chunk */
    int rc = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
    DIE(rc < 0, "recvfrom");

    /* TODO B.2: Write the chunk to the file */
    write(fd, buffer, rc);

    /* TODO B.3: Exit the loop when the file was received entirely */
    if (rc == 0) break;
  }

  // Close the file
  close(fd);
}

void recv_and_send_a_message(int sockfd) {
  char *hello = "Reply from the server";
  char buffer[MAXLINE];
  struct sockaddr_in cliaddr;
  socklen_t len = sizeof(cliaddr);
  memset(&cliaddr, 0, sizeof(cliaddr));

  int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                   (struct sockaddr *)&cliaddr, &len);
  if (n < 0) {
    perror("recvfrom");
    exit(EXIT_FAILURE);
  } else {
    // Add \0 to the end of the received message
    buffer[n] = '\0';

    // Print the received message
    printf("Received %d bytes from %s: %s\n", n, inet_ntoa(cliaddr.sin_addr),
           buffer);
  }

  n = sendto(sockfd, (const char *)hello, strlen(hello), 0,
             (const struct sockaddr *)&cliaddr, len);
  DIE(n < 0, "sendto");
  printf("Reply message sent.\n");
}

int main(int argc, char *argv[]) {
  int sockfd;
  struct sockaddr_in servaddr;

  // Creating socket file descriptor
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  if (argc < 2) {
    fprintf(stderr, "Usage: ./%s <port>\n", argv[0]);
    fprintf(stderr, "Eg: ./%s 10001\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Filling server information
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET; // IPv4
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(atoi(argv[1]));

  // Bind the socket with the server address
  int rc = bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
  DIE(rc < 0, "bind failed");

  /* TODO A: Read the above and test if you can recv and send a message. */
  //recv_and_send_a_message(sockfd);

  /* TODO B: Implement the function to recv a file.
        The line above (calling recv_and_send_a_message) should be comented when
     solving this TODO.
    */

  recv_a_file(sockfd, SAVED_FILENAME);

  // Close the socket
  close(sockfd);

  return 0;
}
