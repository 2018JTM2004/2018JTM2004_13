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


   n = readsock(sock,username); username[n-1] = '\0';
   printf("Username provided : %s\n",username);

   n = readsock(sock,password); password[n-1] = '\0';
   printf("Password provided : %s\n",password);


   //Reading username and password from file user_pass
   FILE * fp = fopen("user_pass","r"); fscanf(fp, "%s %s",name,pass);
   printf("%s %s\n",name,pass);

   if (strcmp(username,name) == 0 && strcmp(password,pass) == 0 ){

     n = write(sock,"granted",7); if (n < 0) error("ERROR writing to socket");
     printf("%s authenticated!\n",username);
     char message[11]= "Logged In!"; writesock(sock,message);

     readsock(sock,buffer); // Reading Input string from Client
     printf("%s : %s\n",username, buffer );
     results(buffer); //process the string
     printf("Server : %s\n",buffer );
     writesock(sock,buffer); //Processed string sent to client.

   }

   else{
     n = write(sock,"refused",strlen(buffer)); if (n < 0) error("ERROR writing to socket");
     printf("%s failed to authenticate!\n",username);

   }
}
