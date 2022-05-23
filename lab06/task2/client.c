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

#include "common.h"
#include "utils.h"

#define MAXLINE 1024
#define CHUNKSIZE 1024
#define SENT_FILENAME "file.bin"

void send_seq_packet(int sockfd, struct sockaddr_in server_address,
                     struct seq_udp *seq_packet) {

  static uint32_t next_seq = 0; // HINT: foloseste variabila asta pentru a retine la
                                // ce nr de secventa esti

  //TODO: nr de secventa in seq_packet
  seq_packet->seq = next_seq;

  //Trimitem packetul la server
  int rc = sendto(sockfd, seq_packet, sizeof(struct seq_udp), 0,
                  (struct sockaddr *)&server_address, sizeof(server_address));
  DIE(rc < 0, "send");

  uint32_t ack;
  rc = recvfrom(sockfd, &ack, sizeof(ack), 0, NULL, NULL);
  if (rc < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      /* TODO: Trateaza situatie timeout ack */
      send_seq_packet(sockfd, server_address, seq_packet);
    } else {
      DIE(rc < 0, "recv ack");
    }
  } else {
    // s-a primit ACK
    if (ack < next_seq) {
      send_seq_packet(sockfd, server_address, seq_packet);
    } else if (ack == next_seq) {
      next_seq++;
    }
  }
}

void send_file(int sockfd, struct sockaddr_in server_address, char *filename) {
  int fd = open(filename, O_RDONLY);
  DIE(fd < 0, "open");

  while (1) {
    struct seq_udp p;
    int n = read(fd, p.payload, sizeof(p.payload));
    DIE(n < 0, "read");

    p.len = n;
    send_seq_packet(sockfd, server_address, &p);

    if (n == 0) // end of file
      break;
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

  /* RO_RCVTIMEO makes recv to timeout if no data received
   TODO: Uncomment the code bellow once you handled this case. */
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));

  // Filling server information
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  int rc = inet_aton(argv[1], &servaddr.sin_addr);
  if (rc == 0) {
    fprintf(stderr, "ERROR: %s is not a valid IP address.\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  send_file(sockfd, servaddr, SENT_FILENAME);

  close(sockfd);
  return 0;
}
