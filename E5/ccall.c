#include <stdlib.h>
#include <stdio.h>
#include <err.h>

int
main (int argc, char * argv[]){
  printf("PROGRAMA CCALL\n===============\n\n");

  if(argc > 2){
    errx(1,"Too much arguments.Example: ./ccall [path]");
  }else{
    if(argc == 1){
      printf("El directorio actual\n");
    }else{
      printf("El directorio por argumento\n");
    }
    exit(0);
  }
}
