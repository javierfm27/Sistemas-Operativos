#/bin/sh
./compiler.sh ccall.c
./executable /tmp/testdir
sleep 5
./borrar.sh
