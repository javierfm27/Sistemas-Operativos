#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

static int permsDir;
static int permsFich;

static int
checkargv(char * fichname){
  struct stat st;

  if(stat(fichname,&st) < 0){
    return -1;
  }
  if(S_ISREG(st.st_mode)){
    return 1;
  }else if(S_ISDIR(st.st_mode)){
    return 0;
  }
  return -1;
}


static int
copiaFich(char * fichSrc, char * fichDst)
{
  struct stat stDst;
  int fdDst;

  //Lo primero es comprobar el fichero Destino
  if(stat(fichDst,&stDst) < 0){
    //No existe, creamos dicho fichero con sus permisos

    if((fdDst = open(fichDst,O_WRONLY | O_TRUNC | O_CREAT,permsFich))< 0 ){
      warn("Error en Open al Crear: %s",fichDst);
    }

    if(close(fdDst) < 0){
      warn("close: %s",fichDst);
    }
  }
  return -1;
}


int
main (int argc, char * argv[]){
  printf("UNICA FUNCION DE STDIO QUE UTILIZAREMOS PARA VER TRAZAS\n");
  int x;

  // ¡¡¡¡¡¡RECUERDA, NO UTILICES NINGUNA FUNCION DE STDIO,QUITA TODOS LOS PRINTFS !!!!!!!
  if(argc == 5){
    x = checkargv(argv[3]);
    permsDir = atoi(argv[1]);
    permsFich = atoi(argv[2]);
    if (x < 0){
      warn("checkargv: %s",argv[3]); //Comprobamos si es directorio o fichero lo que nos estan proporcionando
    }else if(x > 0){
      printf("Fichero\n");
      if(copiaFich(argv[3],argv[4]) < 0){
        errx(1,"Error al copiar Fichero %s",argv[3]);
      }
    }else{
      printf("Directorio Recursivo\n");
    }
  }else{
    errx(1,"[octal permission][octal permission][src path o file][dest path o file]");
  }
}
