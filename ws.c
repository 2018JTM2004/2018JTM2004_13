/* To run the server program
* 1. make -B
* 2. Example:  ./server.out 12345
*/

/*
* The program assumes a file nammed
* user_pass exists with username and
* password arranged in two columns.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> //for read and write function

void attend(int);
void results(char *query);
int readsock(int sock,char buffer[]);
int writesock(int sock, char buffer[]);

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int readsock(int sock,char buffer[]){
  int n;
  bzero(buffer,256);
  n = read(sock,buffer,256);
  if (n < 0) error("ERROR reading from socket");
  return n;
}

int writesock(int sock, char buffer[]){
  int n;
  n = write(sock,buffer,strlen(buffer));
  if (n < 0) error("ERROR writing to socket");
  return n;
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen, pid;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
    else{
      printf("Socket Binded to Port : %d\n", portno);
    }
     listen(sockfd,2);
     printf("Listening for connections\n");
     clilen = sizeof(cli_addr);
     while (1) {

         // Accept new connection.
         newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) error("ERROR on accept");
         pid = fork();
         if (pid < 0) error("ERROR on fork");
         if (pid == 0)  {
             printf("Connection Accepted\n");
             close(sockfd);
             attend(newsockfd);
             exit(0);
         }
         else close(newsockfd);
       }
     return 0;
}
void results(char s[]){

//This function changes case of each character.
  int c = 0;
  char ch;

  while (s[c] != '\0') {
     ch = s[c];
     if (ch >= 'A' && ch <= 'Z')
        s[c] = s[c] + 32;
     else if (ch >= 'a' && ch <= 'z')
        s[c] = s[c] - 32;
     c++;
  }

  // printf("%s\n", s);


}
void attend (int sock)
{
   int n;
   //Initialize array to hold username, password and other data.
   char buffer[256],username[256],password[256];
   char name[10],pass[10];

   auth:
   //Read username from socket
   n = readsock(sock,username); username[n-1] = '\0';
   printf("Username provided : %s\n",username);

   //Read password from socket
   n = readsock(sock,password); password[n-1] = '\0';
   printf("Password provided : %s\n",password);


   //Reading username and password from file user_pass
   FILE * fp = fopen("user_pass","r"); fscanf(fp, "%s %s",name,pass);
   // printf("%s %s\n",name,pass);

   // Compare username and password with Credentials from file.
   if (strcmp(username,name) == 0 && strcmp(password,pass) == 0 ){

     n = write(sock,"granted",strlen("granted")); if (n < 0) error("ERROR writing to socket");
     printf("%s granted access!\n",username);

     while(1){
       readsock(sock,buffer); // Reading Input string from Client
       printf("%s : %s\n",username, buffer );
       results(buffer); //process the string
       printf("server : %s\n",buffer );
       writesock(sock,buffer); //Processed string sent to client.
    }
   }

   else{
     n = write(sock,"refused",strlen("refused")); if (n < 0) error("ERROR writing to socket");
     printf("%s failed to authenticate!\n",username);
     goto auth;

   }
}
