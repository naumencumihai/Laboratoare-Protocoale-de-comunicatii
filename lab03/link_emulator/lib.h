#ifndef LIB
#define LIB

#include <stddef.h>

#define MAX_LEN 1500

typedef struct {
  int len;
  char payload[MAX_LEN];
} msg;

void init(char* remote,int remote_port);
void set_local_port(int port);
void set_remote(char* ip, int port);
int link_recv(void *buf, int len);
void link_send(void *buf, int len);

#endif
