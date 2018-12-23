/* To run the client program
* first run
* 1. make -B
* 2. ./server.out 12345
* 3. Example:  ./client.out anshuman 12345
* Port no provided to client should be same
* as given to server.
*/

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
// function to read from socket
int readsock(int sock,char buffer[]){
  int n;
  bzero(buffer,256);
  n = read(sock,buffer,255);
  if (n < 0) error("ERROR reading from socket");
  return n;
}
// function to read from standard Input
void readstdin(char buffer[]){

  bzero(buffer,256);
  fgets(buffer,255,stdin);

}
// function to write into socket
int writesock(int sock, char buffer[]){
  int n;
  n = write(sock,buffer,strlen(buffer));if (n < 0) error("ERROR writing to socket");
  return n;
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    // Defining structs to store addresss
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    //Username and Password Section
    auth:
    printf("Username : ");
    readstdin(buffer);
    writesock(sockfd,buffer); //Username sent
    printf("Password : ");
    readstdin(buffer);
    writesock(sockfd,buffer); //Password sent

    readsock(sockfd,buffer); printf("%s\n",buffer);//Check login status.

    if (strcmp(buffer,"granted") == 0){

        while (1) {
          printf("Input Query : ");
          readstdin(buffer);
          buffer[strlen(buffer)-1]  = '\0';
          writesock(sockfd,buffer); //Input Query sent
          if (strcmp(buffer,"q") == 0){
            break;
          }
          else {
            readsock(sockfd,buffer);
            printf("Processed Query : %s\n",buffer);
          }

        }

    }

    else if(strcmp(buffer,"refused") == 0){
        printf("Invalid Credentials!\n");
        goto auth;

    }
    else{
      printf("Something went terribly wrong. Try restarting Client.\n" );
    }

    return 0;
}
