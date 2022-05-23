#include "common.h"

uint16_t inet_csum(uint8_t *buf, size_t len) {
	//TODO1: Calculate the internet checksum according to RFC1071

	uint32_t sum = 0;

	while (len > 1) {
		sum += *((uint16_t*)buf);
		buf += 2;
		len -= 2;
	}

	if (len > 0) {
		sum += *((uint8_t*) buf);
	}

	while (sum>>16) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}

	return(~sum);
}
