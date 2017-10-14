#include <stdlib.h>
#include <stdio.h>


int
main (int argc, char * argv[])
{
  char num[] = "555";
  unsigned long int permission;

  permission = strtouq(num,NULL,10);
  printf("Estos son los permisos %lu\n",permission);
}
