#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

struct ficherosCompilados{
  char fichname[50];
  int pid;
};
typedef struct ficherosCompilados ficherosCompilados;

ficherosCompilados * arrayStruct[];

//---------------------------------------------------------------------------------------------------------------------------------
void
compileFich(char * fichname,char * dir)
{
  int pid, sts;
  char nameExec[100];
  char * pathFich;

  //Nombre ejecutable
  strncpy(nameExec,fichname,(strlen(fichname - 2)));
  strcat(nameExec,"\0");
  //Path Fichero
  pathFich = malloc(strlen(dir) + 1 + strlen(fichname));
  strcpy(pathFich,dir);
  strcat(pathFich,"/");
  strcat(pathFich,fichname);
  //Hacemos el fork
  printf("NameExec: %s\n",nameExec);
  printf("Path.C: %s\n\n",pathFich);
  pid = fork();
  switch (pid) {
    case -1:
      warn("fork: ");
    case 0:
      execl("/usr/bin/gcc","gcc","-o",nameExec,pathFich,NULL);
    default:
      //Hacemos el Free y deberiamos añadirlo al arrayStruct
      free(pathFich);
  }
}
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
    //Llamamos al programa que compila
    if(strstr(de->d_name,".c") != NULL){
      printf("dir: %s\n",path);
      compileFich(de->d_name,path);
    }
    continue;
  }

  //Close DIR
  if(closedir(f) < 0){
    warn("closedir in leerDir: ");
  }
  return 0;
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
