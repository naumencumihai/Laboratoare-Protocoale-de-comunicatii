#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "link_emulator/lib.h"

uint16_t inet_csum(uint8_t *buf, size_t len);

/* Layer 3 header */
struct l3_msg_hdr {
	uint16_t len;
	uint16_t sum;
};

/* Layer 3 frame */
struct l3_msg {
	struct l3_msg_hdr hdr;
	/* Data */
	char payload[sizeof(((msg *) NULL)->payload) / 2 - sizeof(struct l3_msg_hdr)];
};
