all: server.out client.out
server.out:
	gcc -o s server.c
client.out:
	gcc -o c client.c
