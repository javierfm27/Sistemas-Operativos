#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//CONSTANTES
#define SIZEBUF 8*1024
#define MAX_ARGS 30
#define MAX_LENGTH 10

//Tipo Comando
struct typecomando{
  char * argvs[MAX_ARGS];
  char cmd[MAX_LENGTH];
};
typedef struct typecomando typecomando;

//Tipo Linea
struct typeLine{
  char linea[SIZEBUF];
  typecomando comando;
  FILE * stdin;
  FILE * stdout;
};
typedef struct typeLine typeLine;


//############################################################################
//Funciones
int
readLine(typeLine * lineaCmd){
  char *linea;

  if(fgets((*lineaCmd).linea, SIZEBUF, stdin) == NULL){
    return -1;
  }

  //Obtenida la linea vamos a separar comando y argumentos
  linea = (*lineaCmd).linea;
  strtok(linea," ");
  strcpy((*lineaCmd).comando.cmd, linea);
  //Ahora debemos obtener los argumentos de cada Comando
  int i = 0;
  while(linea != NULL){
    linea = strtok(NULL, " ");
    (*lineaCmd).comando.argvs[i] = linea;
    i++;
  }
  return 0;
}

int
executeCmd(typeLine * comando){
  int n;
  n = fork();
  switch (n) {
    case -1:
      err(1,"Error Al crear el Hijo");
      return -1;
    case 0:

    default:

  }
}

//Principal
int
main (int argc, char * argv[]){
  typeLine lineaCmd;

  for(;;){
    printf("myShell@#: ");
    if(readLine(&lineaCmd) == 0){
      executeCmd(&lineaCmd);
    }else{
      err(1,"Error al leer");
    }
  }
  exit(0);
}
