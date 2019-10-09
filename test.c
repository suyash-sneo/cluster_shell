#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
  
int execute(char* command){
    //printf("Executing for %d\n", getpid());
    char * comm;
    char * token;
    char * args[20];
    int i = 1;
    comm = strtok(command, " ");
    args[0] = (char*)malloc(strlen(comm));
    strcpy(args[0],comm);
    token = strtok(NULL, " ");
    while(token != NULL){
        args[i] = (char*)malloc(strlen(token));
        strcpy(args[i],token);
        token = strtok(NULL, " ");
        i++;
    }
    args[i] = malloc(sizeof(char));
    args[i] = 0;
    execvp(comm, args);
}

void main(){
    int fd = open("hello.txt", O_CREAT|O_WRONLY, 0666);
    //printf()
    close(1);
    dup(fd);
    char test[] = "ls";
    execute(test);
}