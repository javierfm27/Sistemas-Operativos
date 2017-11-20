#!/bin/sh

#Lo primero es comprobar que nos pasan como argumentos 4 ficheros
if test $# -eq 4
then
  touch notasfinales.txt
else
  echo usage error: $0 [file1.txt] [file2.txt] [file3.txt] [file4.txt]
  exit 1
fi
#Nos quedamos con todos los alumnos, en una variable para luego asi saber quien no realizo la entrega de algun ejercicio
#comando-> awk '$0 ~ /^#/ {next} {print $1}' $1 | sort
for i in $*
do
  awk '$0 ~ /^#/ {next} {printf ("%s\n",$1)}' $i | sort -u >>alumnos.txt
done
alumnos=`cat alumnos.txt | sort -u`
rm alumnos.txt
#Ya tenemos los alumnos, ahora vamos a hacer nuestro fichero de notasfinales
echo "#Nombre   Ejer1   Ejer2   Ejer3   Ejer4   Final" >notasfinales.txt
#Vamos a conseguir las notas
for i in $alumnos
do
  echo $i >>notasfinales.txt
  awk '$0 ~ /'$i'/ {print $2}' $* >>notasfinales.txt
done
