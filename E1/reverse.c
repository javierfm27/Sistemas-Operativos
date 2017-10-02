#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

void
reverse(char * p){
  long int x;
  int i;

  x = strlen(p);
  for (i = x; i >= 0; i--){
    printf("%c", p[i]);
  }
  printf("\n");
}

int
main (int argc, char * argv[]){
  int i;

  if (argc == 1){
    err(1,"Debe introducir argumentos, ./reverse [arg1] [arg2]");
  }else{
    for(i = 1; i < argc; i++){
      reverse(argv[i]);
    }
    exit(0);
  }
}
