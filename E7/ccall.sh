#!/bin/sh

ficherosC=*.c
dflag=n
cflag=n
wordflag=n
echo $#
if test $# -eq 0
then
  for i in $ficherosC
  do
    gcc -c $i
  done
else
  for i in $@
  do
    echo bucle $i
    case $i in
      *-d*)
        echo entra en d
        dflag=y
        shift
        echo despues de asignar $#
        directorio=$1;;
      *-c*)
        echo entra en c
        cflag=y
        shift
        options=$1;;
      *)
        wordflag=y
        word=$1;;
    esac
    if test $# -eq 1
    then
      break
    else
      shift
    fi
  done
fi
echo $directorio
echo $word
echo $options
#Ahora hacemos las compilaciones
if test [$cflag = y] && [$dflag = y]
then
  echo si
fi
exit 0
