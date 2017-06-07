#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <ctype.h>
#include <errno.h>

struct Contador {
	char	Caracter;
	int	Freq;
};
typedef  struct Contador Contador;


int 
main (int argc , char* argv[])
{
	char buffer[1024];
	char str[] = "-f"; //Variable para comparar el -f
	int nr , i , j , z , x , fd;
	Contador Caracteres[128];  // Como tiene que ser un caracter de la tabla ASCII, como maximo tendre 128
	
	
	fd = 0;
	if ( argc > 1 ) {
		for (i = 0; i < 128; i++) {
			Caracteres[i].Caracter = 0;
			Caracteres[i].Freq = 0;
		};
		for (i = 1; i < argc; i++) {
			x = strlen(argv[i]);
			if (x == 1) {
				Caracteres[i - 1].Caracter = *argv[i];
				Caracteres[i - 1].Freq = 0;
			}else{
				if (strcmp(str,argv[i]) == 0) {
					fd = open(argv[i+1],O_RDONLY);
					if (fd < 0 ) {
						err(errno, "%s", argv[i+1]);
					};
				};
			};
		};
		
		for (;;) {
			nr = read(fd, buffer , sizeof buffer);
			if (nr == 0){
				break;
			}else if (nr > 0) {
				for (j = 0; j < nr; j++) {
					for (z = 0; z < 128; z++) {
							if (Caracteres[z].Caracter == buffer[j]) {
								Caracteres[z].Freq = Caracteres[z].Freq + 1;
								break;
							} else if (buffer[j] < 33) {
								break;
							};
					};	
				};
			}else if (nr < 0) {
				err(errno, "read");
			};
		};
		for (i = 0; i < 128; i++) {
			if (Caracteres[i].Caracter != 0) {
				printf("%c: %d \n", Caracteres[i].Caracter, Caracteres[i].Freq);
			};
		};
		if (fd > 2 ){
			close(fd);
		};
		exit(0);

	}else{	
		errx(0,"Solo se ha introducido el nombre del programa\n");
	}
	
}
