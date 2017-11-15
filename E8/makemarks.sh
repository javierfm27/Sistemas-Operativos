#!/bin/sh

#Lo primero es comprobar que nos pasan como argumentos 4 ficheros
if test $# -eq 4
then
  touch notasfinales.txt
else
  echo usage error: $0 [file1.txt] [file2.txt] [file3.txt] [file4.txt]
fi
