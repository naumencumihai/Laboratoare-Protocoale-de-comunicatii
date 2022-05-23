// Protocoale de comunicatii
// Laborator 11 - DNS
// dns.c

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int usage(char* name)
{
	printf("Usage:\n\t%s -n <NAME>\n\t%s -a <IP>\n", name, name);
	return 1;
}

// Receives a name and prints IP addresses
void get_ip(char* name)
{
	int ret;
	struct addrinfo hints, *result, *p;

	// TODO: set hints
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;

	// TODO: get addresses
	result = malloc(sizeof(result));
	if (!result) return;

	p = result;

	ret = getaddrinfo(name, NULL, &hints, &result);
	if (ret != 0) {
 		printf("[ERROR] Couldn't get address info\n");
		return;
	}

	// TODO: iterate through addresses and print them
	for (p = result; p != NULL; p = p->ai_next) {
		void *addr;
		char ip[100];

		if (p->ai_family == AF_INET) {
		addr = &((struct sockaddr_in*) p->ai_addr)->sin_addr;
		} else if (p->ai_family == AF_INET6) {
		addr = &((struct sockaddr_in6*) p->ai_addr)->sin6_addr;
		}

		if (inet_ntop(p->ai_family, addr, ip, sizeof(ip)) != NULL) {
		printf("IPv%d address: %s Canon name: %s Protocol: %d\n",
				p->ai_family == PF_INET6 ? 6 : 4, ip, p->ai_canonname, p->ai_protocol);

		}
	}

	// TODO: free allocated data
	freeaddrinfo(result);
}

// Receives an address and prints the associated name and service
void get_name(char* ip)
{
	int ret;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(struct sockaddr);
	char host[1024];
	char service[20];

	// TODO: fill in address data
	addr.sin_family = AF_INET;
   	addr.sin_port = htons(8080);
   	inet_aton(ip, &addr.sin_addr);

	// TODO: get name and service
	ret = getnameinfo((struct sockaddr *)&addr, addrlen, 
						host, sizeof(host), 
						service, sizeof(service), 
						0);
	if (ret != 0) {
		printf("[ERROR] Couldn't get name info\n");
		return;
	}

	// TODO: print name and service
	printf("Host: %s\tService: %s\n", host, service);
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		return usage(argv[0]);
	}

	if (strncmp(argv[1], "-n", 2) == 0) {
		get_ip(argv[2]);
	} else if (strncmp(argv[1], "-a", 2) == 0) {
		get_name(argv[2]);
	} else {
		return usage(argv[0]);
	}

	return 0;
}
