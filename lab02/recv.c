#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

#define DLE (char)0
#define STX (char)2
#define ETX (char)3

#define MAX_SIZE 100

/* TODO 2: write recv_frame function */

int recv_frame(char *buffer, int max_size)
{
      /* Am primit începutul de frame? */
      char c1, c2;
      char* d = "DONE";
 
      c1 = recv_byte();
      c2 = recv_byte();
 
      while((c1 != DLE) && (c2 != STX)) {
        c1 = c2;
        c2 = recv_byte();
      }

      printf("%d ## %d\n", c1, c2);

      for (int i = 0; i < max_size; i++) {
        char byte = recv_byte();
        //printf("%d--", byte);
 
        if (byte == DLE) {
          byte = recv_byte();
 
          if (byte == ETX)
            return i;
 
          else if (byte != DLE)
            return -1;
        }
 
        buffer[i] = byte;

        if (strstr(buffer, d) != NULL) {
          return i;
        }
      }
 
      return max_size;
}

int main(int argc,char** argv){
  init(HOST,PORT);
  
  // char c;

  // /* Wait for the start of a frame */
  // char c1,c2;
  // do {
	// c1 = recv_byte();
	// c2 = recv_byte();
  // } while(c1 != DLE || c2 != STX);
  
  // printf("%d ## %d\n",c1, c2);
  // c = recv_byte();
  // printf("%c\n", c);

  // c = recv_byte();
  // printf("%c\n", c);

  // c = recv_byte();
  // printf("%c\n", c);

  // c = recv_byte();
  // printf("%c\n", c);

  // c = recv_byte();
  // printf("%c\n", c);

  // c = recv_byte();
  // printf("%c\n", c);
  

  /* TODO 2: Run the receiver until you receive the frame DONE */

  char *str;

  str = (char*)malloc(MAX_SIZE);

  recv_frame(str, MAX_SIZE);

  printf("%s\n", str);

  /* TODO 3: receive a frame with a structure of type Packet */



  /* TODO 4: Measure latency in a while loop for any frame that contains a timestamp we receive, print frame_size and latency */

  printf("[RECEIVER] Finished transmission\n");
  return 0;
}
