#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "link_emulator/lib.h"

/* We don't touch this */
#define HOST "127.0.0.1"
#define PORT 10000

#define DLE (char)0
#define STX (char)2
#define ETX (char)3

struct Packet {
	int   size; /* size inainte lui payload pentru a sti dinainte dimensiunea */
	char  payload[30];
     int   sum;
}; 

/* TODO 2: implement send_frame function */

void send_frame(char *frame, int frame_size)
{
   /* Trimite delimitator */
   send_byte(DLE);
   send_byte(STX);
 
   /* Trimite bytes din frame */
   for (int i = 0; i < frame_size; i++) {
      /* Facem escape la escape */
      if (frame[i] == DLE)
        send_byte(DLE);
 
      send_byte(frame[i]);
   }
 
   /* Trimite delimitator final */
   send_byte(DLE);
   send_byte(ETX);
}

int main(int argc,char** argv){
	init(HOST,PORT);

	/* Send Hello 
	send_byte(DLE);
	send_byte(STX);
	send_byte('H');
	send_byte('e');
	send_byte('l');
	send_byte('l');
	send_byte('o');
	send_byte('!');
	*/

	send_frame("Laborator - DONE - nu apare", 27);

	/* TODO 2: call send_frame function with a given string input */

	/* TODO 3: use send_frame to send a structure of type Packet */

	/* TODO 4: send 100 bytes, send 300 bytes, append a timestamp to these frames */	
	
	return 0;
}
