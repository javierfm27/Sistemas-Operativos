#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>




static int
leerfichero(char * fichname){
  int fd, br;
  char buf[1024];

  printf("es lo que tenemos %s \n",fichname);
  fd = open(fichname, O_RDONLY);
  if (fd < 0){
    err(errno, "open in leerfichero: ");
  }
  //Bucle que leera del fichero
  for(;;){
    br = read(fd,buf,sizeof(buf));
    if (br == 0){
      break;
    }
    if(write(0,buf,sizeof(buf)) != br){
      break;
    }
  }
  if(close(fd) < 0){
    err(errno,"close in leerfichero: ");
  };
  return 0;
}




static int
leerdirectorio(char * dir){
  DIR * d;
  struct dirent * x;


if((strcmp(dir,".") == 0) | (strcmp(dir,"..") ==0) ){
    return 0;
  }
  d = opendir(dir);
  if (d == NULL){
    warn("opendir: %s",dir);
  }
  //Bucle que ira llamando recursivamente
  while ((x = readdir(d)) != NULL){
    printf("%s\n",x->d_name);
    if(x->d_type == DT_REG){
      leerfichero(x->d_name);
    }else if(x->d_type == DT_DIR){
      leerdirectorio(x->d_name);
    }
  }
  //Aqui cerramos Directorio
  if (closedir(d) < 0){
    warn("closedir: ");
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
  //Directorio por argumento
  leerdirectorio(argv[1]);
  exit(0);
}
