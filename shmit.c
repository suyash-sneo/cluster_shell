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
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/shm.h>



void shmit(char **incmd, int infd, int outfd, char **outcmd);


int main(int argc,char *argv){


 	int infd;
 	if((infd=open("i.txt",O_RDONLY))==-1)
 		perror("open infd: ");
 	int outfd;
 	//if((outfd=open("o.txt",O_RDWR | O_CREAT))==-1)
 	//	perror("open outfd: ");
 	outfd=-1;
 	//infd=-1;
 	char *incmd[]={"wc",(char *)0};
 	char *outcmd[]={"wc",(char *)0};
 	shmit(incmd,infd,outfd,outcmd);
 	printf("main done\n");
}


void shmit(char **incmd, int infd, int outfd, char **outcmd){

	key_t key = ftok("shmit.c",1337); 
	int shmid = shmget(key, 1024,0666 | IPC_CREAT); 
	printf("shmid :%d  key :%d\n",shmid,key);

	int pid;
	int status;

	if((pid=fork())==0){

		int pid2;
		int status2;
		int pipefd[2];

		pipe(pipefd);

		if((pid2=fork())==0){
			
			close(pipefd[0]);
			if(infd!=-1)
				dup2(infd,STDIN_FILENO);
			dup2(pipefd[1],STDOUT_FILENO);
			execvp(incmd[0],incmd);
		}

		wait(&status2);
		printf("Child 1-1 pid :%d\tstatus :%d\n",pid2,status2);
		close(pipefd[1]);	

		char *s,*shmptr;
		if((s=shmat(shmid,NULL,0))==(char *)-1)
			perror("shmat incmd: ");
		shmptr=s;
		char c;
		while((c=read(pipefd[0],&c,1))!=0){
			*shmptr=c;
			shmptr++;
		}
		*shmptr=NULL;	
		shmdt(s);
		exit(0);
	}

	wait(&status);
	printf("Child 1 pid: %d\tstatus: %d\n",pid,status);

	if((pid=fork())==0){

		int pid2;
		int status2;
		int pipefd[2];

		pipe(pipefd);
		//close(pipefd[0]);

		char *s,*shmptr;
		char c;
		int x;

		if((s=shmat(shmid,NULL,0))==(char *)-1)
			perror("shmat incmd: ");
		shmptr=s;
		//printf("gg\n");
		while((char)*shmptr !=NULL){
			c=(char) *shmptr;
			//printf("aa\n");
			if((x=write(pipefd[1],&c,1))==-1)
				perror("write child 2");
			//printf("bb\n");
			shmptr++;
		}
		//printf("cc\n");
		shmdt(s);
		shmctl(shmid,IPC_RMID,NULL); 
		printf("shm removed\n");
		if((pid2=fork())==0){

			close(pipefd[1]);
			if((outfd=open("o.txt",O_RDWR | O_CREAT))==-1)
 				perror("open outfd: ");
			if(outfd!=-1)
				dup2(outfd,STDOUT_FILENO);
			dup2(pipefd[0],STDIN_FILENO);
			execvp(outcmd[0],outcmd);

		}
		close(pipefd[1]);
		wait(&status2);
		printf("Child 2-1 pid :%d\tstatus: %d\n",pid2,status2);
		exit(0);
	}

	wait(&status);
	if(WIFSIGNALED(status))
		printf("Child 2 terminated by signal\n");
	printf("Child 2 pid: %d\tstatus: %d\n",pid,status);
}	