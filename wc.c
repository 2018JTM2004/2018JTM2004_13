#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h> //for read and write function
void error(char *msg);
int readsock(int sock,char buffer[]);
int writesock(int sock, char buffer[]);
void readstdin(char buffer[]);

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
void readstdin(char buffer[]){

  bzero(buffer,256);
  fgets(buffer,255,stdin);

}
int writesock(int sock, char buffer[]){
  int n;
  n = write(sock,buffer,strlen(buffer));if (n < 0) error("ERROR writing to socket");
  return n;
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    //Username and Password Section
    printf("Username : ");
    readstdin(buffer);
    writesock(sockfd,buffer); //Username sent
    printf("Password : ");
    readstdin(buffer);
    writesock(sockfd,buffer); //Password sent

    readsock(sockfd,buffer);
    printf("%s\n",buffer);
    
    readsock(sockfd,buffer);printf("%s\n",buffer);//Print successful login message.


    printf("Input Query : ");
    readstdin(buffer);
    writesock(sockfd,buffer); //Input Query sent
    readsock(sockfd,buffer);
    printf("Processed Query : %s",buffer);

    return 0;
}
