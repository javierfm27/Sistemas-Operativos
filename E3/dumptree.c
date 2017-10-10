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

static char *
makepath (char * directorio, char * nameDir)
{
  int size;
  char * path;

  if (nameDir != NULL){
    size = strlen(directorio) + 1 + strlen(nameDir) + 1;
    path = malloc(size);
    strcpy(path,directorio);
    strcat(path,"/");
    strcat(path,nameDir);
    return path;
  }else{
    size = strlen(directorio);
    path = malloc(size);
    strcpy(path,directorio);
    return path;
  }
}


static int
leerfichero(char * fichname)
{
  int fd, br;
  char buf[1024];

  fd = open(fichname, O_RDONLY);
  if (fd < 0){
    warn("open: %s\n",fichname);
  }
  for(;;){
    br = read(fd,buf,sizeof(buf));
    if(br==0){
      break;
    }
    if (write(0,buf,br) != br){
      break;
    }
  }

  if(close(fd) < 0){
    warn("close: %s\n",fichname);
  }
  return 0;
}




static int
leerdirectorio(char * dir,char * name)
{
  DIR * d;
  struct dirent * x;
  char * path;

  //Comprobamos que no es ni ".", ni ".."
  if (name != NULL){
    if((strcmp(name,".") == 0) | (strcmp(name,"..") ==0) ){
        return 0;
    }
  }
  path = makepath(dir,NULL);
  d = opendir(path);
  if (d == NULL){
    warn("opendir: %s", path);
  }

  //Bucle que llamara de manera recursiva
  while((x = readdir(d)) != NULL){
    path = makepath(dir, x->d_name);
    if (((strcmp(x->d_name,"..")) == 0) | ((strcmp(x->d_name,".")) == 0)){
      continue;
    }else{
      printf("%s\n",path);
    }
    if(x->d_type == DT_DIR){
      leerdirectorio(path,x->d_name);
    }else if(x->d_type == DT_REG){
      leerfichero(path);
    }
    free(path);
  }


  //Cerramos Directorio
  if (closedir(d) < 0){
    warn("closedir: ");
  }
  return 0;
}



int
main (int argc, char * argv[])
{
  char dir[1*1024];

  if(argc == 1){
    getcwd(dir,sizeof(dir));
    if (dir == NULL){
      warn("Get Current Dir Name: ");
    }
    printf("%p\n",dir);
    leerdirectorio(dir,NULL);
  }else if (argc > 2){
    printf("The program only accepts one directory\n");
    exit(1);
  }
  //Directorio Por argumento
  if(leerdirectorio(argv[1],NULL) < 0){
    exit(1);
  }
  exit(0);
}
