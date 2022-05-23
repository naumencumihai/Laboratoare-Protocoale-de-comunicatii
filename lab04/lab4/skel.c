#include "skel.h"

extern int interfaces[ROUTER_NUM_INTERFACES];

int get_sock(const char *if_name)
{
	int res;
	int s = socket(AF_PACKET, SOCK_RAW, 768);
	DIE(s == -1, "socket");

	struct ifreq intf;
	strcpy(intf.ifr_name, if_name);
	res = ioctl(s, SIOCGIFINDEX, &intf);
	DIE(res, "ioctl SIOCGIFINDEX");

	struct sockaddr_ll addr;
	memset(&addr, 0x00, sizeof(addr));
	addr.sll_family = AF_PACKET;
	addr.sll_ifindex = intf.ifr_ifindex;

	res = bind(s , (struct sockaddr *)&addr , sizeof(addr));
	DIE(res == -1, "bind");
	return s;
}

msg* socket_receive_message(int sockfd, msg *m)
{
	/*
	 * Note that "buffer" should be at least the MTU size of the
	 * interface, eg 1500 bytes
	 * */
	m->len = read(sockfd, m->payload, MAX_LEN);
	DIE(m->len == -1, "read");
	return m;
}

int send_packet(int sockfd, msg *m)
{        
	/* 
	 * Note that "buffer" should be at least the MTU size of the 
	 * interface, eg 1500 bytes 
	 * */
	return write(interfaces[sockfd], m->payload, m->len);
}

int get_packet(msg *m) {
	int res;
	fd_set set;

	FD_ZERO(&set);
	while (1) {
		for (int i = 0; i < ROUTER_NUM_INTERFACES; i++) {
			FD_SET(interfaces[i], &set);
		}

		res = select(interfaces[ROUTER_NUM_INTERFACES - 1] + 1, &set, NULL, NULL, NULL);
		DIE(res == -1, "select");

		for (int i = 0; i < ROUTER_NUM_INTERFACES; i++) {
			if (FD_ISSET(interfaces[i], &set)) {
				socket_receive_message(interfaces[i], m);
				m->interface = i;
				return 0;
			}
		}
	}
	return -1;
}

int get_interface_mac(int interface, uint8_t *mac)
{
	struct ifreq ifr;
	sprintf(ifr.ifr_name, "r-%u", interface);
	ioctl(interfaces[interface], SIOCGIFHWADDR, &ifr);
	memcpy(mac, ifr.ifr_addr.sa_data, 6);
	return 1;
}

void init()
{
	int s0 = get_sock("r-0");
	int s1 = get_sock("r-1");
	int s2 = get_sock("r-2");
	int s3 = get_sock("r-3");
	interfaces[0] = s0;
	interfaces[1] = s1;
	interfaces[2] = s2;
	interfaces[3] = s3;
}

uint16_t ip_checksum(void* vdata,size_t length) {
	// Cast the data pointer to one that can be indexed.
	char* data=(char*)vdata;

	// Initialise the accumulator.
	uint64_t acc=0xffff;

	// Handle any partial block at the start of the data.
	unsigned int offset=((uintptr_t)data)&3;
	if (offset) {
		size_t count=4-offset;
		if (count>length) count=length;
		uint32_t word=0;
		memcpy(offset+(char*)&word,data,count);
		acc+=ntohl(word);
		data+=count;
		length-=count;
	}

	// Handle any complete 32-bit blocks.
	char* data_end=data+(length&~3);
	while (data!=data_end) {
		uint32_t word;
		memcpy(&word,data,4);
		acc+=ntohl(word);
		data+=4;
	}
	length&=3;

	// Handle any partial block at the end of the data.
	if (length) {
		uint32_t word=0;
		memcpy(&word,data,length);
		acc+=ntohl(word);
	}

	// Handle deferred carries.
	acc=(acc&0xffffffff)+(acc>>32);
	while (acc>>16) {
		acc=(acc&0xffff)+(acc>>16);
	}

	// If the data began at an odd byte address
	// then reverse the byte order to compensate.
	if (offset&1) {
		acc=((acc&0xff00)>>8)|((acc&0x00ff)<<8);
	}

	// Return the checksum in network byte order.
	return htons(~acc);
}

static int hex2num(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}
int hex2byte(const char *hex)
{
	int a, b;
	a = hex2num(*hex++);
	if (a < 0)
		return -1;
	b = hex2num(*hex++);
	if (b < 0)
		return -1;
	return (a << 4) | b;
}
/**
 * hwaddr_aton - Convert ASCII string to MAC address (colon-delimited format)
 * @txt: MAC address as a string (e.g., "00:11:22:33:44:55")
 * @addr: Buffer for the MAC address (ETH_ALEN = 6 bytes)
 * Returns: 0 on success, -1 on failure (e.g., string not a MAC address)
 */
int hwaddr_aton(const char *txt, uint8_t *addr)
{
	int i;
	for (i = 0; i < 6; i++) {
		int a, b;
		a = hex2num(*txt++);
		if (a < 0)
			return -1;
		b = hex2num(*txt++);
		if (b < 0)
			return -1;
		*addr++ = (a << 4) | b;
		if (i < 5 && *txt++ != ':')
			return -1;
	}
	return 0;
}

size_t read_nei_table(struct nei_entry *nei_table)
{
	fprintf(stderr, "Parsing neighbors table\n");

	FILE *f = fopen("nei_table.txt", "r");
	DIE(f == NULL, "Failed to open nei_table.txt");

	char line[100];
	size_t i;

	for (i = 0; fgets(line, sizeof(line), f); i++) {
		char ip_str[50], mac_str[50];

		if (strchr(line, '.') == NULL)
			nei_table[i].proto = 6;
		else
			nei_table[i].proto = 4;

		sscanf(line, "%s %s", ip_str, mac_str);
		fprintf(stderr, "IPv%d: %s MAC: %s\n", nei_table[i].proto, ip_str, mac_str);

		if (nei_table[i].proto == 4) {
			int rc = inet_pton(AF_INET, ip_str, &nei_table[i].ip);
			DIE(rc != 1, "invalid IPv4");
		}

		if (nei_table[i].proto == 6) {
			int rc = inet_pton(AF_INET6, ip_str, &nei_table[i].ip6);
			DIE(rc != 1, "invalid IPv6");
		}

		int rc = hwaddr_aton(mac_str, nei_table[i].mac);
		DIE(rc < 0, "invalid MAC");
	}

	fclose(f);
	fprintf(stderr, "Done parsing neighbors table.\n");

	return i;
}

size_t read_rtable(struct rtable_entry *rtable)
{
	fprintf(stderr, "Parsing routing table\n");

	FILE *f = fopen("rtable.txt", "r");
	DIE(f == NULL, "Failed to open rtable.txt");

	char line[200];
	size_t i;

	for (i = 0; fgets(line, sizeof(line), f); i++) {
		char net_str[50], nh_str[50], nm_str[50];

		if (strchr(line, '.') == NULL)
			rtable[i].proto = 6;
		else
			rtable[i].proto = 4;

		int rc = sscanf(line, "%s %s %s %u %d", net_str, nm_str, nh_str, &rtable[i].metric, &rtable[i].interface);
		fprintf(stderr, "IPv%d route: %s/%s via %s dev r-%d metric %u\n", rtable[i].proto, net_str, nm_str, nh_str, rtable[i].interface, rtable[i].metric);
		DIE(rc != 5, "invalid routing table entry");

		if (rtable[i].proto == 4) {
			rc = inet_pton(AF_INET, net_str, &rtable[i].network);
			DIE(rc != 1, "invalid IPv4");

			rc = inet_pton(AF_INET, nm_str, &rtable[i].netmask);
			DIE(rc != 1, "invalid IPv4");

			rc = inet_pton(AF_INET, nh_str, &rtable[i].nexthop);
			DIE(rc != 1, "invalid IPv4");
		}

		if (rtable[i].proto == 6) {
			rc = inet_pton(AF_INET6, net_str, &rtable[i].network6);
			DIE(rc != 1, "invalid IPv6");

			rc = inet_pton(AF_INET6, nm_str, &rtable[i].netmask6);
			DIE(rc != 1, "invalid IPv6");

			rc = inet_pton(AF_INET6, nh_str, &rtable[i].nexthop6);
			DIE(rc != 1, "invalid IPv6");
		}
	}

	fclose(f);
	fprintf(stderr, "Done parsing routing table.\n");

	return i;
}
