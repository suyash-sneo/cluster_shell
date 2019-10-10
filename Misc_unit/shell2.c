#include<wait.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/errno.h>
#include<unistd.h>
#include<string.h>


#define BLUE printf("\033[0;34m")
#define CYAN printf("\033[1;36m")
#define WHITE printf("\033[0m")
#define BWHITE printf("\033[1;37m")

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
    
    while(1){

        char command[100];
        CYAN;
        printf("$ ");
        WHITE;
        gets(command);
        if((ret_val = fork())>0){
            int status;
            int pid=wait(&status);
            BWHITE;
            printf("\nProcess %d terminated ", pid);
            if(WIFEXITED(status))
                printf("normally\n");
            if(WIFSIGNALED(status))
                printf("by signal\n");
            WHITE;
        }
        else{
            execute(command);
        }
    }
}