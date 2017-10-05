#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

int
leerfichero(char * fichname){
  int fd;
  char buf[1*1024];
  int br;

  fd = open(fichname, O_RDONLY);
  if (fd < 0){
    err(errno, "open in leerfichero: ");
  }
  //Bucle que leera del fichero
  br = read(fd,buf, sizeof buf);
  while(br > 0){
    br = read(fd,buf, sizeof buf);
    write(0, buf, sizeof buf);
  }

  if(close(fd) < 0){
    err(errno,"close in leerfichero: ");
  };
  return 0;
}


int
leerdirectorio(char * dir){
  DIR * d;
  struct dirent * x;

  d = opendir(dir);
  if (d == NULL){
    return 1;
  }
  //Bucle que ira llamando recursivamente
  while ((x = readdir(d)) != NULL){
    if(x->d_type == DT_REG){
      printf("%s\n",x->d_name);
      leerfichero(x->d_name);
    }else if(x->d_type == DT_DIR){
      printf("%s\n",x->d_name);
      printf("TRABAJANDO EN ELLO\n");
    }
  }
  //Aqui cerramos Directorio
  if (closedir(d) < 0){
    return 1;
  }
  return 0;
}





int
main (int argc, char * argv[]){
  char  dir[1*100];

  if (argc == 1){
    if(getcwd(dir,sizeof dir) == NULL){
      err(errno, "getcwd: ");
    }
    if(leerdirectorio(dir) != 0){
      err(errno, "leerdirectorio: ");
    }
    exit(0);
  }else if(argc > 2){
    printf("The program only aceppts one directory.\n");
    exit(1);
  }
  printf("ESTAMOS TRABAJANDO PARA EL DIRECTORIO ACTUAL\n");
}
