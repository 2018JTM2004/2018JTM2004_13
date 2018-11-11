CC=gcc
make: server.out client.out
client.out: 
	$(CC) -o client.out wc.c 
server.out:
	$(CC) -o server.out ws.c 
