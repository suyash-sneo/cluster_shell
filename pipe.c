##include <stdio.h>
#include <unistd.h>                     
#include <stdlib.h>                     
#include <signal.h>                     
#include <time.h>						
#include <stdint.h> 					
#include <sys/time.h>					
#include <sys/wait.h>
#include <pthread.h>
#include <sys/select.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>


void pipeit(char **incmd, int infd, int outfd, char **outcmd);


 int main(int argc,char *argv){


 	int infd;
 	if((infd=open("i.txt",O_RDONLY))==-1)
 		perror("open infd: ");
 	int outfd;
 	if((outfd=open("h.txt",O_WRONLY | O_CREAT))==-1)
 		perror("open outfd: ");
 	//outfd=-1;
 	//infd=-1;
 	char *incmd[]={"wc",(char *)0};
 	char *outcmd[]={"wc",(char *)0};
 	pipeit(incmd,infd,outfd,outcmd);
 	printf("main done\n");
}


void pipeit(char **incmd, int infd, int outfd, char **outcmd){

	int pipefd[2];
	int status;
	int pid;

	if (pipe(pipefd) == -1) perror("pipe : ");

	printf("pipefd = %d %d\n", pipefd[0],pipefd[1]);

	if((pid=fork())==0){
		setpgid(0,0);
		close(pipefd[0]);
		if(infd !=-1){
			dup2(infd,STDIN_FILENO);
		}
		dup2(pipefd[1],STDOUT_FILENO);
		execvp(incmd[0],incmd);
	}

	waitpid(-1,&status,WNOHANG);
	printf("pid :%d\tstatus :%d\n",pid,status);

	if((pid=fork())==0){
		setpgid(0,0);
		close(pipefd[1]);
		if(outfd !=-1){
			dup2(outfd,STDOUT_FILENO);
		}
		dup2(pipefd[0],STDIN_FILENO);
		execvp(outcmd[0],outcmd);
	}

	waitpid(-1,&status,WNOHANG);
	printf("pid :%d\tstatus :%d\n",pid,status);
	close(pipefd[0]);
	close(pipefd[1]);
	wait(&status);
}
