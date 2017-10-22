#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

struct ficherosCompilados{
  char fichname[50];
  int pid;
  struct ficherosCompilados * siguiente;
};

typedef struct ficherosCompilados ficherosCompilados;
ficherosCompilados * lista;
int elementos;

//---------------------------------------------------------------------------------------------------------------------------------
static int
addSon(int pid, char * ejecutable)
{
  ficherosCompilados * p;
  ficherosCompilados * p_nuevo;

  //Comprobamos que este vacia la Lista;
  if(lista == NULL){
    elementos++;
    lista = malloc(sizeof(ficherosCompilados));
    lista->pid = pid;
    strcpy(lista->fichname,ejecutable);
  }else{
    //Rerservamos para el nuevo elemento
    p_nuevo = malloc(sizeof(ficherosCompilados));
    p_nuevo->pid = pid;
    strcpy(p_nuevo->fichname,ejecutable);
    //Buscamos un sitio
    p = lista;
    while(p->siguiente != NULL){
      p = p->siguiente;
    }
    p->siguiente = p_nuevo;
  }
  return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------
static void
borrarLista()
{
  ficherosCompilados * p_aux;
  ficherosCompilados * p_borrado;

  p_aux = lista;
  while(p_aux->siguiente != NULL){
    p_borrado = p_aux;
    p_aux = p_aux->siguiente;
    free(p_borrado);
  }
  free(p_aux);
  free(lista);
}
//---------------------------------------------------------------------------------------------------------------------------------
static void
statusProcess(int pid,int status)
{
  ficherosCompilados * p;
  p = lista;
  while((p->siguiente != NULL) | (pid != p->pid)){
    p = p->siguiente;
  }
  //Ponemos si compila o no
  if(status < 0){
    printf("%s: no compila\n",p->fichname);
  }else{
    printf("%s: compila\n",p->fichname);
  }
}
//---------------------------------------------------------------------------------------------------------------------------------
static void
compileFich(char * fichname,char * dir)
{
  int pid, pidHijo,sts;
  char nameExec[50];
  char * pathFich;
  char * cflag;
  int sizeExec;

  memset(nameExec,0,sizeof(nameExec));
  //Nombre ejecutable
  sizeExec = strlen(fichname) - 2;
  strncpy(nameExec,fichname,sizeExec);
  //Path Fichero
  pathFich = malloc(strlen(dir) + 1 + strlen(fichname));
  strcpy(pathFich,dir);
  strcat(pathFich,"/");
  strcat(pathFich,fichname);
  //Hacemos el fork
  pid = fork();
  switch (pid) {
    case -1:
      warn("fork: ");
    case 0:
      if((cflag = getenv("CFLAGS")) == NULL ){
        execl("/usr/bin/gcc","gcc","-o",nameExec,pathFich,NULL);
      }else{
        execl("/usr/bin/gcc","gcc",cflag,"-o",nameExec,pathFich,NULL);
      }
    default:
      //Hacemos el Free y deberiamos añadirlo al arrayStruct
      free(pathFich);
      //Añadimos al array para luego esperar a los hijos
      if(addSon(pid,fichname) < 0){
        errx(1,"Error al añadir");
      }
      pidHijo = wait(&sts);
      statusProcess(pidHijo,sts);
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
      compileFich(de->d_name,path);
    }
    continue;
    printf("Cuantas veces pasa por aqui");
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
      borrarLista();
    }
    exit(0);
  }
}
