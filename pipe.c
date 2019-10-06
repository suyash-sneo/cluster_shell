#include "header.h"

int main(int argc, char *argv[]){

		int pipefd[2];
		pipe(pipefd);


		int pid=fork();

		if(pid==0){
			close(pipefd[0]);
			dup2(pipefd[1],1);

			int err2;
			if( (err2 = execlp(argv[1],argv[1],(char *)0)) == -1)
			perror("exec: ");
		}
		else{
			close(pipefd[1]);
			int err1,err2;
			if((err1=dup2(pipefd[0],0))==-1)
				perror("dup2: ");
			int status;
			wait(&status);
			if( (err2 = execlp(argv[2],argv[2],(char *)0)) == -1)
				perror("exec: ");
		}
}