#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

static inline uint8_t hamming_4to7(uint8_t c) {
	// TODO 2: Implement hamming encoding for one nibble
	bool d4 = (c>>0) & 1;
	bool d3 = (c>>1) & 1;
	bool d2 = (c>>2) & 1;
	bool d1 = (c>>3) & 1;

	bool p1 = d1 ^ d2 ^ d4;
	bool p2 = d1 ^ d3 ^ d4;
	bool p3 = d2 ^ d3 ^ d4;

    return (p1 << 6) | (p2 << 5) | (d1 << 4) |
    (p3 << 3) | (d2 << 2) | (d3 << 1) | (d4 << 0);

}

/* For each 4 bits we add 3 redundancy bits. We split a byte intwo two */
size_t hamming_encode(uint8_t *buf, size_t len, uint8_t *enc) {
	for (size_t idx = 0; idx < len; idx++) {
		enc[idx * 2] = hamming_4to7(buf[idx] >> 4);
		enc[idx * 2 + 1] = hamming_4to7(buf[idx] & 0xf);
	}

	return len * 2;
}

int main(int argc,char** argv) {
	init(HOST,PORT);

	/* Look in common.h for the definition of l3_msg */
	struct l3_msg t;

	/* We set the payload */
	sprintf(t.payload, "Hello World of PC");
	t.hdr.len = strlen(t.payload) + 1;
	/* Add the checksum */
	t.hdr.sum = inet_csum((void *) t.payload, t.hdr.len);

	/* Encode the message with error correction codes */
	uint8_t enc[2 * (sizeof(t.hdr) + t.hdr.len)];
	hamming_encode((void *) &t, sizeof(enc) / 2, enc);

	/* Send the message */
	link_send(&enc, sizeof(enc));

	return 0;
}
