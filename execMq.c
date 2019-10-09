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
#include<sys/wait.h>

#define MSGQ_PATH "."

typedef struct myMsg{
    long mtype;
    char msg[1001];
}myMsg;

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

void execMq(char* incomm, int flag_in, int infd, char** comm, int count){
    int retval, i=count;
    int msqid = msgget(ftok(MSGQ_PATH, 26), IPC_CREAT|0666);
    retval = fork();
    while(retval!=0 && --i>0)
        retval = fork();
    
    if(retval!=0){
        myMsg list[count];
        int pipes[2];
        pipe(pipes);
        int retval2 = fork();
        if(retval2!=0){
            char BUF[1001];
            wait(NULL);
            close(pipes[1]);
            read(pipes[0], BUF, 1000);
            for(int j=0; j<count; j++){
                list[j].mtype =1;
                strcpy(list[j].msg, BUF);
                msgsnd(msqid, &list[j], strlen(list[j].msg), 0);
            }
        }
        else{
            if(flag_in==1){
                close(0);
                dup(infd);
            }
            close(pipes[0]);
            close(1);
            dup(pipes[1]);
            execute(incomm);
        }
    }
    else{
        char BUF2[1000];
        char * myComm;
        myMsg info;
        int filedes[2];
        pipe(filedes);
        msgrcv(msqid, (void*)&info, 1000, 0, 0);
        write(filedes[1], info.msg, strlen(info.msg)-6);
        close(filedes[1]);
        close(0);
        dup(filedes[0]);
        myComm = (char*)malloc(strlen(comm[i-1]));
        strcpy(myComm, comm[i-1]);
        execute(myComm);
    }
}


void main(){
    printf("%d\n", getpid());
    char* test[] = {"wc", "sort"};
    char test2[] = "ls";
    execMq(test2, 0, 0, test, 2);
}