#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/file.h>
#include <unistd.h>

static char * namefichcontador;
static char * word;
////////////////////////////////////////////////////////////////////////////////
static void
upcontador()
{
  int fd, num, nr;
  char buf[10];

  if ((fd = open(namefichcontador,O_RDWR))<0){
    warn("open");
  }
  if(flock(fd,LOCK_EX)<0){
    warn("flock");
  }
  if((nr=read(fd,buf,sizeof buf))<0){
    warn("read");
  }
  num = atoi(buf);
  num++;
  lseek(fd,0,0);
  sprintf(buf,"%d\n",num);
  if(write(fd,buf,nr) != nr){
    warn("write");
  }
  if(flock(fd,LOCK_UN)<0){
    warn("flock");
  }
}
////////////////////////////////////////////////////////////////////////////////
//FUNCION QUE SERA LA QUE SE LANZE CUANDO HAGAMOS UN THREAD
static void*
checkFile(void * fich)
{
  char * nameFile;
  char line[1024];
  int wordFlag = 0;
  FILE * f;

  nameFile = fich;
  //Abrimos el fichero y leemos linea a linea buscando la palabra
  if((f = fopen(nameFile,"r")) < 0){
    warn("fopen");
  }
  while(fgets(line,sizeof line, f) != NULL){
    if(strstr(line,word)!= NULL){
      wordFlag = 1;
      break;
    }
  }
  fclose(f);

  //Ahora tenemos que actualizar el contador en el  fichero
  if(wordFlag == 1){
    upcontador();
  }
  return 0;
}
////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char * argv[])
{
  int i;
  int contadorHilos;
  int fdcontador;
  pthread_t thr[argc - 2];

  if (argc < 3){
    errx(1,"usage error: [word] [file.txt]");
  }
  argc--;
  argv++;

  //Primero comprobamos que el fichero existe, si no, lo creamos
  namefichcontador = getenv("HOME");
  if(namefichcontador== NULL){
    warn("Error en getenv");
  }
  namefichcontador = strcat(namefichcontador,"/cfiles.cnt");

  if ((fdcontador = open(namefichcontador, O_RDWR | O_CREAT, 0644))<0){
    warn("error in open");
  }
  if(close(fdcontador)< 0){
    warn("Error");
  }

  //Cogemos la palabra objetivo
  word = argv[0];

  //Creamos tantos procesos como ficheros pasemos al programa
  argc--;
  argv++;
  contadorHilos = 0;
  for(i = 0; i < argc; i++){
    if(pthread_create(thr+i,NULL,checkFile,argv[i])!= 0){
      warn("pthread create");
    }
    contadorHilos++;
  }

  //Antes de finalizar, nos aseguramos que los hilos han acabado sus tareas
  for (i = 0; i < contadorHilos; i++){
    pthread_join(thr[i],NULL);
  }
  exit (0);
}
