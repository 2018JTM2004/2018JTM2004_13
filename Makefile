CC=gcc
make: server.out client.out
client.out: 
	$(CC) -o client.out wc2.c 
server.out:
	$(CC) -o server.out ws2.c 
