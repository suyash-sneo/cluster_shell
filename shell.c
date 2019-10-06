#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/errno.h>
#include<unistd.h>
#include<string.h>

int execute(char* command){
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

int main(){
    int ret_val;
    char command[100];
    printf(">> ");
    gets(command);
    if((ret_val = fork())==0){
        wait();
    }
    else{
        execute(command);
    }
}
