#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

//---------------------------------------------------------------------------------------------------------------------------------
static int
leerDir(char * path)
{
  DIR * f;
  struct dirent * de;

  //Abrimos DIR
  if((f = opendir(path)) < 0){
      warn("opendir in leerDir: ");
  }

  //Leemos el directorio
  while((de = readdir(f)) != NULL){
    if(((strcmp(de->d_name,"..")) == 0) | ((strcmp(de->d_name,".")) == 0)){
      continue;
    }
    printf("%s\n",de->d_name);
  }

  //Close DIR
  if(closedir(f) < 0){
    warn("closedir in leerDir: ");
  }
  return -1;
}
//---------------------------------------------------------------------------------------------------------------------------------
int
main (int argc, char * argv[]){
  printf("PROGRAMA CCALL\n===============\n\n");

  if(argc > 2){
    errx(1,"Too much arguments.Example: ./ccall [path]");
  }else{
    if(argc == 1){
      printf("El directorio actual\n");
    }else{
      if(leerDir(argv[1])< 0 ){
        errx(1,"leerDir in main");
      }
    }
    exit(0);
  }
}
