#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>

int
makepath(char * buf, int len){
  char cwd[1*1024];
  char  pid[8];
  char * user;
  pid_t y;

  //Obtenemos el directorio actual
  if (getcwd(cwd, sizeof cwd) == NULL){
    return -1;
  }
  strcpy(buf,cwd);
  strcat(buf, "/");
  strcat(buf, "\0");

  //Añadimos el Usuario y el PID
  user = getenv("USER");
  if( user == NULL){
    return -1;
  }
  strcat(user, ".");
  strcat(buf, user);
  y = getpid();
  if(sprintf(pid,"%d",y) < 0){
    return -1;
  }
  strcat(buf,pid);
  return strlen(buf);
}

int
main (int argc, char * argv[]){
  char buffer[2*1024]; //Buffer de 2kBytes

  if (makepath(buffer, sizeof buffer) < 0){
    errx(1, "makepath: error en la funcion");
    exit(1);
  };
  printf("%s\n", buffer);
  exit(0);
}
