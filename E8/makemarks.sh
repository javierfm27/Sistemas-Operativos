#!/bin/sh

#Lo primero es comprobar que nos pasan como argumentos 4 ficheros
if test $# -eq 4
then
  touch notasfinales.txt
  echo "#Nombre   Ejer1   Ejer2   Ejer3   Ejer4   Final" >notasfinales.txt
else
  echo usage error: $0 [file1.txt] [file2.txt] [file3.txt] [file4.txt]
  exit 1
fi
#Nos quedamos con todos los alumnos, en una variable para luego asi saber quien no realizo la entrega de algun ejercicio
#comando-> awk '$0 ~ /^#/ {next} {print $1}' $1 | sort
x=$x`awk '$0 ~ /^#/ {next} {printf ("%s\n",$1)}' $2 | sort -u`
x=$x`echo -E "\n"`
x=$x`awk '$0 ~ /^#/ {next} {printf ("%s\n",$1)}' $1 | sort -u`
for i in $x
do
  echo $i
done
echo -e 'javi\tSU'
