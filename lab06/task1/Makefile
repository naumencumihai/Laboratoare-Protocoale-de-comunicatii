IP_SERVER=127.0.0.1
PORT=1234

build: server client
CFLAGS = -Wall -Wextra -g

server: server.c utils.h
	gcc $(CFLAGS) server.c -o server

client: client.c utils.h
	gcc $(CFLAGS) client.c -o client

.PHONY: clean run_client run_server random_file

# Construieste un fisier de 1MB (1024B*10^4)==1MB
random_file:
	dd if=/dev/urandom of=file.bin bs=1024 count=10000

# Ruleaza serverul
run_server: server
	./server ${PORT}

# Ruleaza clientul
run_client: client
	./client ${IP_SERVER} ${PORT}

clean:
	rm -f server client file.bin received_file.bin
