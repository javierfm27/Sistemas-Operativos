#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

//Constantes
#define MAX_ARGS 20
#define MAX_CMDS 20
#define MAX_LENGTH 50

//Variables
struct salida{
  int flag;
  char * path;
};
typedef struct salida salida;

struct entrada{
  int flag;
  char * path;
};
typedef struct entrada entrada;

struct cmdline{
  char path[MAX_LENGTH];
  char * argv[MAX_ARGS];
  int argc;
  int pipeflag;
  entrada in;
  salida out;
  int ampersandflag;
};
typedef struct cmdline cmdline;

cmdline arraycmds[MAX_CMDS];
int posarray = 0;

//Directorio HOME
char * pathHome = "/home/javi";

////////////////////////////////////////////////////////////////////////////////
static void
readline(char * linea)
{
  //En esta funcion leemos la linea, decidiendo asi,entradas,pipes,salidas,ampersand
  char * pargv;
  int flagfincargv = 0;
  int i = 0, j = 0;
  char * savelinea;

  arraycmds[posarray].argv[0] = strtok_r(linea," \n\t",&savelinea);
  arraycmds[posarray].argc = 1;
  while((pargv = strtok_r(NULL," \n\t",&savelinea)) != NULL ){
    //1 Token
    for(j = 0; pargv[j] != 0;j++){
      switch (pargv[j]) {
        //Falta pipe,&
        case '>':
          arraycmds[posarray].out.flag = 1;
          if (strcmp(pargv,">") == 0){
            pargv = strtok_r(NULL," \n\t",&savelinea);
            arraycmds[posarray].out.path = pargv;
          }else{
            arraycmds[posarray].out.path = pargv+1;
          }
          flagfincargv = 1;
          break;
        case '<':
          arraycmds[0].in.flag = 1;
          if(strcmp(pargv,"<")== 0){
            pargv = strtok_r(NULL," \n\t",&savelinea);
            arraycmds[0].in.path = pargv;
          }else{
            arraycmds[0].in.path = pargv+1;
          }
          flagfincargv = 1;
          break;
        case '|':
          arraycmds[posarray].pipeflag = 1;
          posarray++;
          readline(savelinea);
          flagfincargv = 1;
          break;
        case '&':
          arraycmds[posarray].ampersandflag = 1;
          flagfincargv = 1;
          break;
      }
    }
    if (flagfincargv == 0){
      arraycmds[posarray].argv[i+1] = pargv;
      arraycmds[posarray].argc++;
      i++;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
static int
findfile(char * bin, int ind)
{
  //Buscamos el built-in o fichero ejecutable
  char * ppath;
  char buf[1024];
  char pathcmd[1024] ;

  strncpy(buf,getenv("PATH"),sizeof buf);
  ppath = strtok(buf,":");
  while(ppath != NULL){
    strcpy(pathcmd,ppath);
    strcat(pathcmd,"/");
    strcat(pathcmd,bin);
    if(access(pathcmd,X_OK)<0){
      ppath = strtok(NULL,":");
    }else{
      strcpy(arraycmds[ind].path,pathcmd);
      return 0;
    }
  }
  return -1;
}
////////////////////////////////////////////////////////////////////////////////
static void
redirectstd(int indice)
{
  //Funcion que redirecciona la salida del comando a ejecutar
  int fdout, fdin;

  if(arraycmds[indice].out.flag == 1){
    fdout = open(arraycmds[indice].out.path,O_RDWR| O_CREAT,0644);
    if (fdout < 0){
      warn("creat new out: ");
    }
    if(dup2(fdout,1)<0){
      warn("duplicate stdin: ");
    }
    if(close(fdout)<0){
      warn("close fd: ");
    }
  }
  if(arraycmds[indice].in.flag == 1){
    fdin = open(arraycmds[indice].in.path,O_RDONLY);
    if(fdin < 0){
      warn("creat new in: ");
    }
    if(dup2(fdin,0)<0){
      warn("dup2: ");
    }
    if(close(fdin)<0){
      warn("close: ");
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
static void
execcmd(int ind)
{
  //Funcion que se encarga de ejecutar comandos
  int pid,sts;
//  int j;
  int fd[2],fd2[2];


  if((findfile(arraycmds[ind].argv[0],ind))< 0){
    fprintf(stderr,"command don't exists %s\n",arraycmds[ind].argv[0]);
  }else{
    if(arraycmds[ind].pipeflag == 1){
      pipe(fd);
    }
    pid = fork();
    switch (pid){
    case -1:
      warn("fork: ");
    case 0:
      redirectstd(ind);
      if(ind == 0 && posarray > 0){
        dup2(fd[1],1);
        close(fd[1]);
      }else if(ind == posarray && posarray > 0){
        dup2(fd[0],0);
        close(fd[0]);
      }else if(ind != posarray && posarray > 0){
        dup2(fd2[0],0);
        close(fd2[0]);
        dup2(fd[1],1);
        close(fd[1]);
      }
      execv(arraycmds[ind].path,arraycmds[ind].argv);
      exit(1);
    default:
      if(ind == 0 && posarray > 0){
        fd2[0] = fd[0];
        close(fd[1]);
      }else if(ind == posarray && posarray > 0){
        close(fd2[0]);
      }else if(ind < posarray && posarray > 0){
        close(fd2[0]);
        fd2[0] = fd[0];
        close(fd[1]);
      }
      if(arraycmds[ind].ampersandflag == 0){
        while(wait(&sts) != pid){
          ;
        }
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
static void
freecmd()
{
  memset(arraycmds,0,sizeof arraycmds);
}
////////////////////////////////////////////////////////////////////////////////
static int
chdirfunc(int posicion)
{
  if(arraycmds[posicion].argc > 2){
    return -1;
  }else{
    if(arraycmds[posicion].argv[1] == NULL){
      chdir(pathHome);
    }else{
      chdir(arraycmds[posicion].argv[1]);
    }
    return 0;
  }
}
////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char * argv[])
{
  char line[100];
  int i;

  //Leemos de la entrada estandar nuestra linea que contendrá el comando y/o opciones y argumentos
  for(;;){
    fprintf(stderr,">$ ");
    if(fgets(line,sizeof line,stdin) != NULL){
      if(strcmp(line,"\n") == 0){
        continue;
      }
      readline(line);
      if (posarray == 0){
        if(strcmp(arraycmds[0].argv[0],"chd") == 0){
          if(chdirfunc(0)<0){
            fprintf(stderr,"To many arguments to chd\n");
          }
        }else{
          execcmd(0);
        }
      }else{
        for(i=0;i<=posarray;i++){
          if(strcmp(arraycmds[i].argv[0],"chd") != 0){
            execcmd(i);
          }
        }
      }
      freecmd();
      posarray = 0;
    }else{
      break;
    }
  }
  exit(0);
}
