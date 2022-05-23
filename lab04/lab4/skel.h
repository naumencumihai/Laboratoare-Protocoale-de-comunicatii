#pragma once

#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
/* According to POSIX.1-2001, POSIX.1-2008 */
#include <sys/select.h>
/* ethheader */
#include <net/ethernet.h>
/* ether_header */
#include <arpa/inet.h>
/* icmphdr */
#include <netinet/ip_icmp.h>
/* ip6_hdr */
#include <netinet/ip6.h>
/* arphdr */
#include <net/if_arp.h>
#include <asm/byteorder.h>

/* 
 *Note that "buffer" should be at least the MTU size of the 
 * interface, eg 1500 bytes 
 */
#define MAX_LEN 1600
#define ROUTER_NUM_INTERFACES 4

#define DIE(condition, message) \
	do { \
		if ((condition)) { \
			fprintf(stderr, "[%d]: %s\n", __LINE__, (message)); \
			perror(""); \
			exit(1); \
		} \
	} while (0)

typedef struct {
	int len;
	char payload[MAX_LEN];
	int interface;
} msg;

struct nei_entry {
	uint16_t proto;
	struct in_addr ip;
	struct in6_addr ip6;
	uint8_t mac[6];
};

struct rtable_entry {
	uint16_t proto;

	struct in_addr network;
	struct in_addr netmask;
	struct in_addr nexthop;

	struct in6_addr network6;
	struct in6_addr netmask6;
	struct in6_addr nexthop6;

	uint32_t metric;

	int interface;
};
extern int interfaces[ROUTER_NUM_INTERFACES];

int send_packet(int sockfd, msg *m);
int get_packet(msg *m);
char *get_interface_ip(int interface);
int get_interface_mac(int interface, uint8_t *mac);
uint16_t ip_checksum(void* vdata,size_t length);
void init();
size_t read_nei_table(struct nei_entry *nei_table);
size_t read_rtable(struct rtable_entry *rtable);
/**
 * hwaddr_aton - Convert ASCII string to MAC address (colon-delimited format)
 * @txt: MAC address as a string (e.g., "00:11:22:33:44:55")
 * @addr: Buffer for the MAC address (ETH_ALEN = 6 bytes)
 * Returns: 0 on success, -1 on failure (e.g., string not a MAC address)
 */
int hwaddr_aton(const char *txt, uint8_t *addr);

