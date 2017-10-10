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
    return directorio;
  }
}


static int
leerfichero(char * fichname)
{
  int fd, br;
  char buf[1024];

  printf("FICH para leer: %s\n",fichname);
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
  d = opendir(dir);
  if (d == NULL){
    warn("opendir: %s", dir);
  }

  //Bucle que llamara de manera recursiva
  while((x = readdir(d)) != NULL){
    path = makepath(dir, x->d_name);
    if ((strcmp(x->d_name,"..")) != 0){
      printf("%s\n",path);
    }
    if(x->d_type == DT_DIR){
      leerdirectorio(path,x->d_name);
    }else if(x->d_type == DT_REG){
      leerfichero(path);
    }
    free(path);
  }

  if (closedir(d) < 0){
    warn("closedir: ");
  }
  return 0;
}



int
main (int argc, char * argv[])
{
  char * dir;

  if(argc == 1){
    dir = get_current_dir_name();
    if (dir == NULL){
      warn("Get Current Dir Name: ");
    }
    leerdirectorio(dir,NULL);
    free(dir);
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
