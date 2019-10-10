#include <stdio.h>
#include <unistd.h>                     
#include <stdlib.h>                     
#include <signal.h>                     
#include <time.h>						
#include <stdint.h> 					
#include <sys/time.h>					
#include <sys/wait.h>
#include <pthread.h>
#include <sys/select.h>

typedef BLUE printf("\033[0;34m")
typedef WHITE printf("\033[0m")

typedef struct command {
	
	struct command *next;
	char **argv;
	int argc;
	char *infile;
	char *outfile;		// in case 
	int flag1;			// if job is supposed to be run in bg, i.e. '&' encountered
	int flag2;			//0 for nothing. 1 means output redirection to a pipe. 2 for msgQ. 3 for shm
} cmd;