#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int 
main (int argc, char* argv[])
{
	char *temp;
	char *user;
	temp = getenv("TMPDIR");
	user = getenv("USER");
	if (temp == NULL) {
		if (user == NULL ) {
			errx(errno," 'The enviroment variable don't exist' ");
			exit(1);
		}
		errx(errno,"/tmp/%s.%d \n" , user, getpid());
		exit(1);
	} else {
		printf("%s/%s.%d \n", temp, user, getpid());
		exit(0);
	}
	
	
}
