#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 4950 // define port number.
#define BUFSIZE 1024 // define buffer size.
void error(char *msg)
{
    perror(msg);
    exit(1);
}
// read from sock
int readsock(int sock,char buffer[]){
  int n;
  bzero(buffer,256);
  n = read(sock,buffer,256);
  if (n < 0) error("ERROR reading from socket");
  return n;
}
// write into sock
int writesock(int sock, char buffer[]){
  int n;
  n = write(sock,buffer,strlen(buffer));
  if (n < 0) error("ERROR writing to socket");
  return n;
}
//store chat_history in file.
void store_history(char chat[])
{
	time_t mytime;
	mytime = time(NULL);
	FILE *fs;
	fs = fopen("chat_history.txt","a");
	fprintf(fs, "%s %s", chat,ctime(&mytime));
	fclose(fs);
}
//function to braodcast to every user.
void send_to_all(int j, int i, int sockfd, int nbytes_recvd, char *recv_buf, fd_set *master)
{

	if (FD_ISSET(j, master)){
		if (j != sockfd && j != i) {
			if (send(j, recv_buf, nbytes_recvd, 0) == -1) {
				perror("send");
			}
		}
	}
}
//receive a new connection or data from clients.
void send_recv(int i, fd_set *master, int sockfd, int fdmax,char *argv[])
{
	int nbytes_recvd, j;
	char recv_buf[BUFSIZE],chat[BUFSIZE];

	if ((nbytes_recvd = recv(i, recv_buf, BUFSIZE, 0)) <= 0) {
		if (nbytes_recvd == 0) {
			printf("socket %d hung up\n", i);
		}else {
			perror("recv");
		}
		close(i);
		FD_CLR(i, master);
	}else {
		store_history(recv_buf);
		// char *c = malloc(strlen(recv_buf)+strlen(argv[1])+1);
		// strcpy(c,recv_buf);
	  // strcat(c,argv[1]);
		// strcpy(recv_buf,c);
		for(j = 0; j <= fdmax; j++){
			send_to_all(j, i, sockfd, nbytes_recvd, recv_buf, master );
		}
	}
}
//accept connection from clients.
void connection_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr)
{
	char username[256],password[256];
	char name[10],pass[10];

	socklen_t addrlen;
	int newsockfd,n;

	addrlen = sizeof(struct sockaddr_in);
	if((newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen)) == -1) {
		perror("accept");
		exit(1);
	}else {
		auth:
    //Read username from socket
    n = readsock(newsockfd,username); username[n-1] = '\0';
    printf("Username provided : %s\n",username);
		//Read password from socket
    n = readsock(newsockfd,password); password[n-1] = '\0';
    printf("Password provided : %s\n",password);
		//Reading username and password from file user_pass
    FILE * fp = fopen("user_pass","r"); fscanf(fp, "%s %s",name,pass);
    // Compare username and password with Credentials from file.
    if (strcmp(username,name) == 0 && strcmp(password,pass) == 0 )
		{

	    n = write(newsockfd,"Logged In!",strlen("Logged In!")); if (n < 0) error("ERROR writing to socket");
	    printf("%s granted access!\n",username);
			FD_SET(newsockfd, master);
			if(newsockfd > *fdmax)
			{
				*fdmax = newsockfd;
			}
			printf("new connection from %s on port %d \n",inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
		}
		else
		{
			n = write(newsockfd,"refused",strlen("refused")); if (n < 0) error("ERROR writing to socket");
			printf("%s failed to authenticate!\n",username);
			goto auth;

		}

		FD_SET(newsockfd, master);
		if(newsockfd > *fdmax){
			*fdmax = newsockfd;
		}
		printf("new connection from %s on port %d \n",inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
	}
}
//creating socket and binding.
void connect_request(int *sockfd, struct sockaddr_in *my_addr)
{
	int yes = 1;

	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(4950);
	my_addr->sin_addr.s_addr = INADDR_ANY;
	memset(my_addr->sin_zero, '\0', sizeof my_addr->sin_zero);

	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	if (bind(*sockfd, (struct sockaddr *)my_addr, sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind");
		exit(1);
	}
	if (listen(*sockfd, 2) == -1) {
		perror("listen");
		exit(1);
	}
	printf("\nTCPServer Waiting for client on port 4950\n");
	fflush(stdout);
}
int main(int argc, char *argv[])
{
	fd_set master;
	fd_set read_fds;
	int fdmax, i;
	int sockfd= 0;
	struct sockaddr_in my_addr, client_addr;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	connect_request(&sockfd, &my_addr);
	FD_SET(sockfd, &master);

	fdmax = sockfd;
	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}

		for (i = 0; i <= fdmax; i++){
			if (FD_ISSET(i, &read_fds)){
				if (i == sockfd)
					connection_accept(&master, &fdmax, sockfd, &client_addr);
				else
					send_recv(i, &master, sockfd, fdmax,argv);
			}
		}
	}
	return 0;
}
