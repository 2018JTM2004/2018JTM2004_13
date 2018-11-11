/* A simple server in the internet domain using TCP
   The port number is passed as an argument
   This version runs forever, forking off a separate
   process for each connection
   gcc server2.c -lsocket
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> //for read and write function

void dostuff(int); /* function prototype */
void results(char *query);
void reads(int sock,char buffer[]);
void writes(int sock, char buffer[]);

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void readsock(int sock,char buffer[]){
  int n;
  bzero(buffer,256);
  n = read(sock,buffer,255);
  if (n < 0) error("ERROR reading from socket");

}

void writes(int sock, char buffer[]){
  int n;
  n = write(sock,buffer,256);
  if (n < 0) error("ERROR writing to socket");

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
     listen(sockfd,2);
     clilen = sizeof(cli_addr);
     while (1) {
         newsockfd = accept(sockfd,
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0)
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             dostuff(newsockfd);
             exit(0);
         }
         else close(newsockfd);

     } /* end of while */
     return 0; /* we never get here */
}
void results(char s[]){
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
void dostuff (int sock)
{
   int n;
   char buffer[256],username[256],password[256];
   char name[10],pass[10];
   bzero(buffer,256);

   bzero(username,256);
   n = read(sock,username,255);
   // printf("%d\n",n);
   if (n < 0) error("ERROR reading from socket");
   printf("Username provided : %s\n",username);
   username[n-1] = '\0';
   // printf("%lu\n",strlen(username));

   bzero(password,256);
   n = read(sock,password,255); if (n < 0) error("ERROR reading from socket");
   printf("Password provided : %s\n",password);
   password[n-1] = '\0';


   FILE * fp;
   fp = fopen("user_pass","r");
   fscanf(fp, "%s %s",name,pass);
   // printf("%s %s\n",name,pass);

   if (strcmp(username,name) == 0 && strcmp(password,pass) == 0 ){
     printf("%s authenticated!\n",username);
     n = write(sock,"Logged In!",18); if (n < 0) error("ERROR writing to socket");


     bzero(buffer,256);
     n = read(sock,buffer,255);
     printf("Query string is %s\n",buffer );
     results(buffer);
     // printf("%s\n",buffer);
     n = write(sock,buffer,256); if (n < 0) error("ERROR writing to socket");



   }
   else{
     printf("Authentication failed! Try Again\n");

   }
}
