#include <stdlib.h>
#include <stdio.h>

struct enteros{
  int x;
  struct enteros * next;
}enteros;

struct enteros * lista;

int
main(int argc, char * argv[])
{
  int size;
  struct enteros * p;

  //Añadir
  if(lista == NULL){
    lista = malloc(sizeof(enteros));
    lista->x = 5;
  }
  printf("%p y %d\n",lista,lista->x);

  free(lista);
}
