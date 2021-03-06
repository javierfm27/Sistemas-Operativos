#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
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
    elementos++;
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

  if (lista != NULL){
    if (elementos == 1){
      free(lista);
    }else{
      p_aux = lista;
      while(p_aux->siguiente != NULL){
        p_borrado = p_aux;
        p_aux = p_aux->siguiente;
        free(p_borrado);
      }
      free(p_aux);
      free(lista);
    }
  }
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
  if(status > 0){
    printf("%s: no compila\n",p->fichname);
  }else{
    printf("%s: compila\n",p->fichname);
  }
}
//---------------------------------------------------------------------------------------------------------------------------------
static void
egrepFork(char * word)
{
  int fd[2];
  int pid;

  if(pipe(fd) < 0){
    warn("Error in pipe");
  }
  pid = fork();
  switch(pid){
    case -1:
      warn("fork: ");
    case 0:
      dup2(fd[0],0);
      close(fd[0]);
      execl("/bin/egrep","egrep",word,NULL);
    default:
      dup2(fd[1],2);
      close(fd[1]);
  }
}
//---------------------------------------------------------------------------------------------------------------------------------
static void
compileFich(char * fichname,char * dir,char * word)
{
  int pid;
  char nameExec[50];
  char * pathFich;
  char * cflag;
  int sizeExec;
  int pidSon;
  int sts;
  int flagWord;

  flagWord = strcmp(word,"");
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
      if (flagWord != 0){
        egrepFork(word);
      }
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
      pidSon = wait(&sts);
      statusProcess(pidSon,sts);
  }
}
//---------------------------------------------------------------------------------------------------------------------------------
static int
leerDir(char * path,char * word) {
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
    //Hay que ver si no se llama redir.c
    if(strcmp(de->d_name,"redir.c") == 0){
      continue;
    }
    //Llamamos al programa que compila
    if(strstr(de->d_name,".c") != NULL){
      compileFich(de->d_name,path,word);
    }
  }

  //Close DIR
  if(closedir(f) < 0){
    warn("closedir in leerDir: ");
  }
  return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------
static int
isDirectory(char * argumento){
  struct stat st;

  if(stat(argumento,&st) < 0){
    return -1;
  }
  if(S_ISDIR(st.st_mode)){
    return 0;
  }else{
    return 1;
  }
}
//---------------------------------------------------------------------------------------------------------------------------------
int
main (int argc, char * argv[]){
  char buf[1024];
  int isDir;
  char word[20];

  memset(word,0,sizeof word);
  if(getcwd(buf,sizeof buf) == NULL){
      warn("getcwd: ");
  }
  if(argc > 3){
    errx(1,"Too much arguments.Example: ./redir [path][word]");
  }else{
    if(argc == 1){
      if(leerDir(buf,word)< 0){
        errx(1,"leerDir in main");
      }
      borrarLista();
    }else if(argc == 2){
      isDir = isDirectory(argv[1]);
      if (isDir == 0){
        if(leerDir(argv[1],word)< 0 ){
          errx(1,"leerDir in main");
        }
      }else{
        strcpy(word,argv[1]);
        if(leerDir(buf,word) < 0){
          errx(1,"leerDir in main");
        }
      }
      borrarLista();
    }else{
      if(leerDir(argv[1],argv[2]) < 0 ){
        errx(1,"leerDir in main");
      }
    }
    exit(0);
  }
}
