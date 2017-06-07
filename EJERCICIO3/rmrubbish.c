#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>

int
compare(struct dirent* p){
	char str[] = ".trash";
	char *pchar;
	char pto [] = ".";
	char ptopto [] = "..";
	int n ,P,P2;

	P = strcmp(pto,p->d_name);
	P2 = strcmp(ptopto,p->d_name);
	
	if ( P == 0 || P2 == 0){
		n = 1;
	}
	pchar = strrchr(p->d_name, '.');
	if (pchar == NULL) {
		return n;
	}
	if (strlen(pchar) <= strlen(str)) {
		n = strcmp(pchar,str);
		if (n != 0){
				n = 1;
		}
	}else{
		n = 1;
	}
	return n;
}

void 
borrado(struct dirent *p, char* directorio){
	int n ;
	char *dir;
	
	dir = directorio;
	strcat(directorio,"/");
	strcat(directorio,p->d_name);
	switch(fork()){
		case -1:
			err(errno,"Fork Failed");
			exit(1);
		case 0:
			execl("/usr/bin/sha512sum","sha512sum",directorio,NULL);
	}
	wait(&n);
	if (n == 0) {
			if (unlink(directorio) < 0){
				fprintf(stderr,"%d: error removing %s \n",getpid(),directorio);
				exit(1);
			}
			fprintf(stderr,"%d: %s ok\n",getpid(),dir);
			exit(0);
	}
}


int
main (int argc,char* argv[]) {
	int n = 0,x,pid,z,ctrl_failed = 0,j = 0;
	DIR *d;
	struct dirent *direct;
	
	if (argc == 1) { 
		errx(errno,"Debe introducir un directorio\n");
		exit(1);
	}
	for (int i = 1; i < argc; i++){
		n = fork();
		j = n + j;
		switch(n){
			case -1:
				err(errno,"Fork Failed");
				exit(1);
			case 0:
				d = opendir(argv[i]);
				if (d == NULL) {
					err(errno, "opendir");
				}
				while((direct = readdir(d)) != NULL){
					x = compare(direct);
					if ( x == 0){
						borrado(direct,argv[i]);
					}
				}
				if ((direct == NULL) && x == 1) {
					fprintf(stderr,"%d: no files to remove in dir %s \n", getpid(),argv[i]);
					exit(1);
				}
		}		
	}			
	if ( j != 0){			
		while( j != 0) {
			pid = wait(&z);
			j = j - pid;
			if (z != 0) {
				ctrl_failed = ctrl_failed + 1;
			}
		}
		if (ctrl_failed != 0){
			fprintf(stderr,"%d processes failed \n",ctrl_failed);
		}else if (j == 0 && ctrl_failed == 0) {
			fprintf(stderr,"all processes were successful\n");
		}	
	}	
	exit(0);
	
	
}
	
