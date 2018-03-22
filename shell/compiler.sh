#/bin/sh
if test $# = 0
then
  echo ./compiler.sh [program.c]
  exit
fi
gcc -c -Wall -Wshadow $1
gcc -o executable *.o -lpthread
