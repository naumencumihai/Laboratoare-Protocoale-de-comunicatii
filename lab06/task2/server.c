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

#include "common.h"
#include "utils.h"

#define SAVED_FILENAME "received_file.bin"

void recv_seq_udp(int sockfd, struct seq_udp *seq_packet) {
  static uint32_t next_seq = 0; // HINT: Foloseste aceasta variabila pentru a retine la
                              // ce numar de secventa esti
  struct sockaddr_in client_addr;
  socklen_t clen = sizeof(client_addr);
  int rc = recvfrom(sockfd, seq_packet, sizeof(struct seq_udp), 0,
                    (struct sockaddr *)&client_addr, &clen);

  // Sending ACK
  rc = sendto(sockfd, &next_seq, sizeof(next_seq), 0,
              (struct sockaddr *)&client_addr, clen);
  DIE(rc < 0, "send");

  if (seq_packet->seq != next_seq) {
    recv_seq_udp(sockfd, seq_packet);
  } else {
    next_seq++;
  }
}

void recv_a_file(int sockfd, char *filename) {
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  DIE(fd < 0, "open");
  struct seq_udp p;
  while (1) {
    /* Receive a chunk */
    recv_seq_udp(sockfd, &p);

    /* Break if file ended */
    if (p.len == 0)
      break;

    /* Write the chunk to the file */
    write(fd, p.payload, p.len);
  }

  close(fd);
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

  /* RO_RCVTIMEO makes recv to timeout if no data received
     TODO: Uncomment the code bellow once you handled this case. */
  // struct timeval tv;
  // tv.tv_sec = 1;
  // tv.tv_usec = 0;
  // setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));

  // Bind the socket with the server address
  int rc = bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
  DIE(rc < 0, "bind failed");

  recv_a_file(sockfd, SAVED_FILENAME);

  return 0;
}
