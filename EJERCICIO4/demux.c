//Programa demux.c - Ejercicio º 4 de Sistemas Operativos
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>

#define MAX_FICH 10
#define MAX_BYTES 10
#define MAX_LENGTH 20

//GLOBALES
struct fichCompress{
  char nombre[MAX_LENGTH];
  int tuberia[2];
};
typedef struct fichCompress fichCompress;

fichCompress ficheros[MAX_FICH];
//FIN GLOBALES

void
addFich(char * nombreFich){
  int i = 0;

  if(ficheros[0].nombre[0] == 0){
    strcpy(ficheros[i].nombre, nombreFich);
    pipe(ficheros[i].tuberia);
  }
  while(ficheros[i].nombre[0] != 0){
    strcpy(ficheros[i].nombre, nombreFich);
    pipe(ficheros[i].tuberia);
    i++;
  }
}

int
compressFile(char * lectura, int i){
  char arg[] = ">";
  int fd[2];

  fd[0] = ficheros[i].tuberia[0];
  fd[1] = ficheros[i].tuberia[1];
  switch(fork()){
    case -1:
      return -1;
    case 0:
      close(fd[0]);
      dup2(fd[1],0);
      close(fd[1]);
      strcat(arg,ficheros[i].nombre);
      execv("/bin/gzip",arg,NULL);
      exit(0);
    default:
      close(fd[0]);
      return 0;
  }
}

void
readBytes(void){
  char bytes[MAX_BYTES];
  char lectura[MAX_BYTES];

  fgets(bytes, sizeof bytes, stdin);
  write(ficheros[0].tuberia[1], bytes, sizeof bytes);
  compressFile(lectura,0);
}

int
main (int argc, char * argv[]){
  int i;

  if (argc < 2){
    errx(1, "demux: Usage error: You need indicate a file to compress");
    exit(1);
  }
  //Añadimos a nuestra lista de ficheros
  for(i = 1; i < argc; i++){
    addFich(argv[i]);
  }

  //Probamos el leerlo una vez
  readBytes();
}
