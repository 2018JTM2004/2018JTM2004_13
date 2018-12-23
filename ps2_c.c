#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define BUFSIZE 1024

void error(char *msg){
    perror(msg);
    exit(0);
}
int readsock(int sock,char buffer[]){
  int n;
  bzero(buffer,256);
  n = read(sock,buffer,255);
  if (n < 0) error("ERROR reading from socket");
  return n;
}
int writesock(int sock, char buffer[]){
  int n;
  n = write(sock,buffer,strlen(buffer));if (n < 0) error("ERROR writing to socket");
  return n;
}
void readstdin(char buffer[]){

  bzero(buffer,256);
  fgets(buffer,255,stdin);

}

void send_recv(int i, int sockfd)
{
	char send_buf[BUFSIZE];
	char recv_buf[BUFSIZE];
	int nbyte_recvd;

	if (i == 0){
		fgets(send_buf, BUFSIZE, stdin);
    send_buf[strlen(send_buf)-1] = '\0';//changed here
		if (strcmp(send_buf , "quit") == 0) { //here also \n removed
			exit(0);
		}else
			send(sockfd, send_buf, strlen(send_buf), 0);
	}else {
		nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
		recv_buf[nbyte_recvd] = '\0';
		printf("%s\n" , recv_buf);
		fflush(stdout);
	}
}


void connect_request(int *sockfd, struct sockaddr_in *server_addr)
{
	char buffer[BUFSIZE];
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(4950);
	server_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(server_addr->sin_zero, '\0', sizeof server_addr->sin_zero);

	if(connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1);
	}
	auth:
	printf("Username : ");
	readstdin(buffer);
	writesock(*sockfd,buffer); //Username sent
	printf("Password : ");
	readstdin(buffer);
	writesock(*sockfd,buffer); //Password sent

	readsock(*sockfd,buffer); printf("%s\n",buffer);//Check login status.
	if (strcmp(buffer,"Logged In!") != 0)
  {
    goto auth;
  }
}

int main()
{
	int sockfd, fdmax, i;
	struct sockaddr_in server_addr;
	fd_set master;
	fd_set read_fds;

	connect_request(&sockfd, &server_addr);
	FD_ZERO(&master);
        FD_ZERO(&read_fds);
        FD_SET(0, &master);
        FD_SET(sockfd, &master);
	fdmax = sockfd;

	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}

		for(i=0; i <= fdmax; i++ )
			if(FD_ISSET(i, &read_fds))
				send_recv(i, sockfd);
	}
	printf("client-quited\n");
	close(sockfd);
	return 0;
}
