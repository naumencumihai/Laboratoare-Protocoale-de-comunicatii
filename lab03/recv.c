#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "common.h"
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

static inline uint8_t hamming_7to4(uint8_t c) {
	// TODO 3: Implement hamming decoding for one nibble
	bool r7 = (c>>0) & 1;
	bool r6 = (c>>1) & 1;
	bool r5 = (c>>2) & 1;
	bool r4 = (c>>3) & 1;
	bool r3 = (c>>4) & 1;
	bool r2 = (c>>5) & 1;
	bool r1 = (c>>6) & 1;

	// TODO 4: Implement error correction

   	bool z1 = r1 ^ r3 ^ r5 ^ r7;
   	bool z2 = r2 ^ r3 ^ r6 ^ r7;
  	bool z3 = r4 ^ r5 ^ r6 ^ r7;

  	uint8_t z = (z3 << 2) | (z2 << 1) | (z1 << 0);
  	/*  Daca z == 0 -> sirul s-a trasnmis cu success
		Daca z != 0 ->
	*/

	if (z != 0) {
		switch (z) {
			case 3:
				r3 ^= 1;
				break;
			case 5:
				r5 ^= 1;
				break;
			case 6:
				r6 ^= 1;
				break;
			case 7:
				r7 ^= 1;
				break;
		}
	}

	return (r3<<3) | (r5<<2) | (r6<<1) | (r7<<0);

}

size_t hamming_decode(uint8_t *enc, size_t len, uint8_t *buf) {
	for (size_t idx = 0; idx < (len / 2); idx++) {
		/* In the encoded message we have to concatenate 4 bits from two different bytes. */
		buf[idx] = hamming_7to4(enc[idx * 2]) << 4;
		buf[idx] |= hamming_7to4(enc[idx * 2 + 1]);
	}

	return len / 2;
}

int main(int argc,char** argv) {
	struct l3_msg t;
	init(HOST,PORT);

	uint8_t enc_ph[sizeof(struct l3_msg) * 2];

	/* Receive the encoded message */
	int len = link_recv(&enc_ph, sizeof(enc_ph));
	if (len < 0){
		perror("Receive message");
		return -1;
	}

	hamming_decode(enc_ph, len, (void *) &t);

	int sum_ok = inet_csum((void *) t.payload, t.hdr.len) == t.hdr.sum;

	printf("len=%d; sum(%s)=0x%04hx; payload=\"%s\";\n", t.hdr.len, sum_ok ? "GOOD" : "BAD", t.hdr.sum, t.payload);

	return 0;
}
