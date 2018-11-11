#include<stdio.h>
void results(char *query);

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
  char name[30] = "Anshuman Singh is my name";
  printf("%s\n",name);
  results(name);
  return 0;
}
