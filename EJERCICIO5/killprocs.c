#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>


int
killproceso(int N ){
  int sts,pid;
  char str[10];
  char * p;

  sprintf(str, "%d",N);
  p = str;
  pid = fork();
  switch(pid){
    case -1:
      fprintf(stderr,"Fork Failed");
      return 1;
    case 0:
      execl("/bin/kill","kill",p,NULL);
      err(errno,"execl");
      exit(1);
    default:
      wait(&sts);
      return sts > 0;
  }
}

FILE *
getfile(void){
  int fd[2],sts;

  if(pipe(fd) < 0 ){
    fprintf(stderr,"Pipe Failed");
  }
  switch(fork()){
    case -1:
      fprintf(stderr,"Fork Failed");
      return NULL;
    case 0:
      close(fd[0]);
      dup2(fd[1],1);
      execl("/bin/ps","ps","ax",NULL);
      fprintf(stderr,"Exec Failed");
      exit(1);
    default:
      close(fd[1]);
      wait(&sts);
      if (sts < 0){
        return NULL;
      }
      return  (fdopen(fd[0],"r"));
  }
}
int
lookprocs(FILE * file,char * proc){
  char str[1024];
  char * token;
  char * token_pid;
  char * saveptr ;
  char * subp;

  while((fgets(str,sizeof(str),file)) != NULL){
    token = strtok_r(str," ",&saveptr);
    token_pid = token;
    while(token != NULL){
      if ((token = strtok_r(NULL," ",&saveptr)) == NULL){
        break;
      }
      if((subp = strstr(token,proc)) != NULL){
        if(atoi(token_pid) != getpid()){
          break;
        }
      }
    }
    if(atoi(token_pid) != getpid()){
      if(subp != NULL){
        break;
      }else{
        token_pid = NULL;
      }
    }
  }
  if (token_pid == NULL){
    return 0;
  }
  return atoi(token_pid);
}


int
main (int argc, char * argv[]){
  int i,pid;
  FILE * pfil;

  if(argc < 2){
    errx(1,"Few arguments [OPTIONS] [PID]");
  }
//WITH OPTION -n
  if (strcmp(argv[1],"-n") == 0){
    if (argc < 3){
      errx(1,"'Need names of processes' killprocs -n [NAME]");
    }
    if((pfil = getfile()) == NULL){
      fprintf(stderr,"No se ha podido obtener los procesos");
      exit(1);
    }
    for(i = 2; i < argc; i++){
      if((pid = lookprocs(pfil,argv[i])) == 0){
        fprintf(stderr,"No existe el proceso %s \n",argv[i]);
        continue;
      }
      if(killproceso(pid) > 0){
        fprintf(stderr,"No se ha podido matar el proceso PID: %d \n",pid);
      }
    }
    exit(0);
  }
  //WITHOUT OPTIONS
  for(i = 1;i < argc; i++){
    if (killproceso(atoi(argv[i])) > 0){
      fprintf(stderr,"No se ha podido matar el proceso PID: %s \n",argv[i]);
    }
  }
  exit(0);

}
