#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
void results(char *query);
int timestamp()
	{
		time_t mytime;
		mytime = time(NULL);
		printf("%s",ctime(&mytime));

		return 0;
	}
  void store_history(char chat[])
  {
  	time_t mytime;
  	mytime = time(NULL);
  	// printf("%s",ctime(&mytime));
  	FILE *fs;
  	fs = fopen("chat_history","a+");
  	fprintf(fs,"%s %s", chat,ctime(&mytime));
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

  printf("%s\n", s);


}
int main(int argc, char const *argv[]) {
  char name[1024] = "Anshuman";
  char recv[1024];
  // strcpy(prefix,name);
  // printf("%s\n",name);
  // printf("%s\n",prefix);
  // char * a = "world";
  // char * b = "Hello";

  char *c = malloc(strlen(name)+strlen(argv[1])+1);
  strcpy(c,name);
  strcat(c,argv[1]);
  strcpy(recv,c);
  printf("%s\n",recv);
  free(c);
  // memcpy(prefix+strlen(argv[1]),argv[1],sizeof(argv[1]));
  // results(name);
  // timestamp();
  // store_history(name);
  return 0;
}
