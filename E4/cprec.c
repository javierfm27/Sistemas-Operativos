#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

mode_t permsDir;
mode_t  permsFich;
//-------------------------------------------------------------------------------------------------------------------------------
static char *
makepath(char * directorio, char * nameDir)
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
    size = strlen(directorio);
    path = malloc(size);
    strcpy(path,directorio);
    return path;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------------------------------------------------
void
copiaFich(char * fichSrc, char * fichDst)
{
  struct stat stDst;
  int fdDst, br, fdSrc;
  char buf[1024];

  //Lo primero es comprobar el fichero Destino
  if(stat(fichDst,&stDst) < 0){
    //No existe, creamos dicho fichero con sus permisos
    if((fdDst = open(fichDst,O_WRONLY | O_TRUNC | O_CREAT,permsFich))< 0 ){
      warn("Error en Open al Crear: %s",fichDst);
    }
  }else{
    if (chmod(fichDst,permsFich) < 0){
      warn("chmod in copiaFich: ");
    }
    if((fdDst = open(fichDst,O_WRONLY|O_TRUNC)) < 0){
      warn("open in chmod : ");
    }
  }
  //Abrimos el origen
  if((fdSrc = open(fichSrc,O_RDONLY)) < 0){
    warn("Error al Abrir: %s",fichSrc);
  }

  //Ahora Realizamos la copia del Fichero
  for(;;){
    br = read(fdSrc,buf,sizeof buf);
    if(br == 0){
      break;
    }
    if (write(fdDst,buf,br) != br){
      break;
    }
  }
  //Cerramos los Ficheros
    if((close(fdDst) < 0) | (close(fdSrc)< 0)){
      warn("close: ");
    }
}
//-------------------------------------------------------------------------------------------------------------------------------
static int
lecturaDir(char * dirSrc, char * dirDst)
{
  DIR * d;
  struct dirent * x;
  char pathPunto[] = ".";
  char pathPuntoPunto[] = "..";
  char * p;
  char * f;
  char * path;
  struct stat st;
  char * pathDestino;

  p = makepath(dirSrc,pathPunto);
  f = makepath(dirSrc,pathPuntoPunto);
  //Comprobamos que no es ni el Dir . ni ..
  if ((strcmp(dirSrc,p) == 0) | (strcmp(dirSrc,f)== 0)){
    return 0;
    free(p);
    free(f);
  }
  //Abrimos el Directorio
  path = makepath(dirSrc,NULL);
  d = opendir(dirSrc);
  if (d == NULL){
    warn("opendir: %s",dirSrc);
  }
  //Bucle que leera el directorio que le pasamos por argumento
  while((x = readdir(d)) != NULL){
    path = makepath (dirSrc, x->d_name);
    pathDestino = makepath(dirDst,x->d_name);
    if  (((strcmp(x->d_name,"..")) == 0) | ((strcmp(x->d_name,".")) == 0)){
      continue;
    }
    if(x->d_type == DT_DIR){
      //Aqui habra una llamada recursiva y hacemos el Dir, comprobamos si existe y cambiamos Permisos
      if(stat(pathDestino,&st) < 0){
        if(mkdir(pathDestino,permsDir) < 0){
          warn("Error al crear directorio: ");
        }
      }else{
        if(chmod(pathDestino,permsDir) < 0){
          warn("chmod in lecturaDir: ");
        }
      }
      lecturaDir(path,pathDestino);
    }else if(x->d_type == DT_REG){
      //Aqui copiamos el fichero
      copiaFich(path,pathDestino);
    }
    free(path);
    free(pathDestino);
  }

  //Cerramos Dir
  if (closedir(d) < 0){
    warn("closedir: ");
  }
  return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------
int
main (int argc, char * argv[]){
  int x;
  struct stat st;

  if(argc == 5){
    x = checkargv(argv[3]);
    permsDir = strtoul(argv[1],NULL,8);
    permsFich = strtoul(argv[2],NULL,8);
    if (x < 0){
      warn("checkargv: %s",argv[3]); //Comprobamos si es directorio o fichero lo que nos estan proporcionando
    }else if(x > 0){
      copiaFich(argv[3],argv[4]);
      exit(0);
    }else{
      if(stat(argv[4],&st) < 0){
        if(mkdir(argv[4],permsDir)<0){
          warn("mkdir in main: ");
        }
      }else{
        if(chmod(argv[4],permsDir) < 0){
          warn("chmod in main: ");
        }
      }
      if(lecturaDir(argv[3],argv[4]) < 0){
        errx(1,"Error al copiar el Directorio %s",argv[3]);
      }
      exit(0);
    }
  }else{
    errx(1,"[octal permission][octal permission][src path o file][dest path o file]");
  }
}
