#/bin/sh
gcc -c -Wall -Wshadow downpath.c
gcc -o downpath downpath.o
./downpath
