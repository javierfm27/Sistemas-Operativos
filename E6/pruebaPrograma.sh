#/bin/sh
./compiler.sh redir.c
./executable /tmp/testdir
sleep 5
./borrar.sh
