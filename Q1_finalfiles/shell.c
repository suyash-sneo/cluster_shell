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
#include <signal.h>
#include<pthread.h>

#define BLUE printf("\033[0;34m")
#define CYAN printf("\033[1;36m")
#define WHITE printf("\033[0m")
#define BWHITE printf("\033[1;37m")
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

void execPipe(char *incomm, int flag_in,int infd,char *outcomm,int flag_out,int outfd,char **comm,int count){

    count+=1;
    int pipefd[count];
    int status;
    int pid[count +1];
    if(outcomm==NULL){
        if(flag_in !=0)
            dup2(infd,STDIN_FILENO);
        if(flag_out !=0)
            dup2(outfd,STDOUT_FILENO);
          
        execute(incomm);
     }
    if (pipe(&pipefd[0]) == -1) perror("pipe incomm: ");

    printf("\npipefd for 1st pipe= %d %d\n", pipefd[0],pipefd[1]);

    if((pid[0]=fork())==0){
        setpgid(0,0);
        close(pipefd[0]);
        if(flag_in !=0){
            dup2(infd,STDIN_FILENO);
        }
        dup2(pipefd[1],STDOUT_FILENO);
        execute(incomm);
        //execvp(incomm[0],incomm);
    }
    

    wait(&status);
    close(pipefd[1]);
    printf("pid :%d\tstatus :%d\n",pid[0],status);
    printf("incomm done\n");

    int  i=1;
    while(count-1>0){

        if(pipe(&pipefd[2*i]) ==-1)
            perror("pipe comm:");
        //close(pipefd[2*i-1]);
        printf("pipefd for %d pipe = %d %d\n",i+1,pipefd[2*i],pipefd[2*i+1]);

        if((pid[i]=fork()) ==0){
            setpgid(0,0);
            //printf("ss\n");
            
            //printf("gg\n");
            dup2(pipefd[2*(i-1)],STDIN_FILENO);
            dup2(pipefd[2*i+1],STDOUT_FILENO);
            //printf("bb\n");
            execute(comm[i-1]);
            
            //execvp(comm[i-1][0],comm[i-1]);
        }
        
        wait(&status);
        printf("comm %d done\n",i-1);
        printf("pid :%d\tstatus :%d\n",pid[i],status);
        close(pipefd[2*(i-1)]);
        close(pipefd[2*i+1]);
        count--;
        i++;
    }

    printf("comm done\n\n");


    if((pid[i]=fork())==0){
        setpgid(0,0);
        close(pipefd[2*i-1]);

        if(flag_out !=0){
            dup2(outfd,STDOUT_FILENO);
        }
        dup2(pipefd[2*(i-1)],STDIN_FILENO);
        execute(outcomm);
        //execvp(outcmd[0],outcmd);
    }

    wait(&status);
    printf("\npid :%d\tstatus :%d\n",pid[i],status);
    close(pipefd[2*(i-1)]);
    //close(pipefd[1]);
    //wait(&status);
    printf("outcomm done\n");
}



//void shmit(char **incmd, int infd, int outfd, char **outcmd)
void execSm(char *incomm, int flag_in, int infd, char **comm, int count){

    key_t key = ftok("shell.c",1337); 
    int shmid = shmget(key, 1024,0666 | IPC_CREAT); 
    printf("\nshmid :%d  key :%d\n",shmid,key);

    int pid[count];
    int status;

    if((pid[0]=fork())==0){

        int pid2;
        int status2;
        int pipefd[2];

        pipe(pipefd);

        if((pid2=fork())==0){
            
            close(pipefd[0]);
            if(flag_in !=0)
                dup2(infd,STDIN_FILENO);
            dup2(pipefd[1],STDOUT_FILENO);
            execute(incomm);
        }

        wait(&status2);
        printf("Child 0-1 pid :%d\tstatus :%d\n",pid2,status2);
        close(pipefd[1]);   

        char *s,*shmptr;
        if((s=shmat(shmid,NULL,0))==(char *)-1)
            perror("shmat incomm: ");
        shmptr=s;
        char c;
        int x;
        while((x=read(pipefd[0],&c,1))!=0){
            *shmptr=c;
            shmptr++;
        }
        *shmptr=NULL;   
        shmdt(s);
        exit(0);
    }

    wait(&status);
    printf("Child 0 pid: %d\tstatus: %d\n",pid[0],status);
    printf("incomm done\n");


    int i=1;
    while(count >0){

        if((pid[i]=fork())==0){

            int pid2;
            int status2;
            int pipefd[2];

            pipe(pipefd);
            

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
            
            if((pid2=fork())==0){

                close(pipefd[1]);
                /*if((outfd=open("o.txt",O_RDWR | O_CREAT))==-1)
                    perror("open outfd: ");
                if(outfd!=-1)
                    dup2(outfd,STDOUT_FILENO);
                */
                dup2(pipefd[0],STDIN_FILENO);
                execute(comm[i-1]);

            }
            close(pipefd[1]);
            wait(&status2);
            printf("Child %d-1 pid :%d\tstatus: %d\n",i,pid2,status2);
            exit(0);
        }
        wait(&status);
        printf("\nChild %d pid: %d\tstatus: %d\n",i,pid[i],status);
        count--;
        i++;
    }

    //wait(&status);
    shmctl(shmid,IPC_RMID,NULL); 
    printf("\nshm removed\n");
    //if(WIFSIGNALED(status))
    //   printf("Child 2 terminated by signal\n");
    
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

int parser(char *command){
    char* command2 = (char*)malloc(strlen(command));
    strcpy(command2, command);
    char* incomm;
    char* outcomm;
    char* p;
    char* token;
    int infd = 0, outfd = 1, flag = 0, flag_in = 0, flag_out = 0;;
    int count=0;

    //Detecting and redirecting control for message queues
    p = strstr(command2, "#");
    while(p != NULL && *p=='#'){
        flag = 1;
        count++;
        p = p+1;
    }
    if(flag==1){
        char hash[count];
        for(int i=0; i<count; i++)
            hash[i] = '#';
        char* comm[count+1];
        incomm = strtok(command2, hash);
        char* temp2 = strtok(NULL, hash);
        token  = strtok(temp2, ",");
        int i=0;
        while(token!=NULL){
            comm[i] = (char*)malloc(sizeof(token));
            strcpy(comm[i], token);
            token = strtok(NULL, ",");
            i++;
        }
        comm[i] = (char*)malloc(1);
        comm[i] = 0;
        p = strstr(incomm, "<");
        if(p!=NULL){
            flag_in = 1;
            token = strtok(incomm, "<");
            char* temp = (char*)malloc(strlen(token));
            strcpy(temp, token);
            incomm = temp;
            token = p+1;
        }
        infd = open(token, O_RDONLY);
        /////////////////////////////////////////////////////////
        execMq(incomm, flag_in, infd, comm, count);
        /////////////////////////////////////////////////////////
        //printf("%s, %d, %d, %d\n", incomm, flag_in, infd, count);
    }

    else{
        //Detecting and redirecting control for shared memory
        p = strstr(command2, "S");
        while(p != NULL && *p=='S'){
            flag = 2;
            count++;
            p = p+1;
        }
        if(flag==2){
            char hash[count];
            for(int i=0; i<count; i++)
                hash[i] = 'S';
            char* comm[count+1];
            incomm = strtok(command2, hash);
            char* temp2 = strtok(NULL, hash);
            token  = strtok(temp2, ",");
            int i=0;
            while(token!=NULL){
                comm[i] = (char*)malloc(sizeof(token));
                strcpy(comm[i], token);
                printf("comms is %s\n", comm[i]);
                token = strtok(NULL, ",");
                i++;
            }
            comm[i] = (char*)malloc(1);
            comm[i] = 0;
            p = strstr(incomm, "<");
            if(p!=NULL){
                flag_in = 1;
                token = strtok(incomm, "<");
                char* temp = (char*)malloc(strlen(token));
                strcpy(temp, token);
                incomm = temp;
                token = p+1;
            }
            infd = open(token, O_RDONLY);
            /////////////////////////////////////////////////////////
            execSm(incomm, flag_in, infd, comm, count);
            /////////////////////////////////////////////////////////
            //printf("%s, %d, %d, %d\n", incomm, flag_in, infd, count);
        }
        else{
            p = strstr(command2, "|");
            if(p!=NULL){
                char* infile;
                char* outfile;
                char* tempcommand = (char*)malloc(strlen(command2));
                strcpy(tempcommand, command2);

                for(int i=0; i<strlen(command2); i++){
                    if(command2[i]=='|')
                        count++;
                }
                char* comm[count];
                token = strtok(tempcommand, "|");
                incomm = (char*)malloc(strlen(token));
                strcpy(incomm, token);
                p = strstr(incomm, "<");
                if(p!=NULL){
                    flag_in = 1;
                    token = strtok(incomm, "<");
                    char* temp = (char*)malloc(strlen(token));
                    strcpy(temp, token);
                    incomm = temp;
                    infile = p+1;
                }
                int i=0;
                token = strtok(command2, "|");
                token = strtok(NULL, "|");
                while(token!=NULL){
                    comm[i] = (char*)malloc(strlen(token));
                    strcpy(comm[i], token);
                    printf("comms is %s\n", comm[i]);
                    token = (strtok(NULL, "|"));
                    i++;
                }
                outcomm = (char*)malloc(strlen(comm[i-1]));
                strcpy(outcomm, comm[i-1]);
                comm[i-1] = (char*)realloc(comm[i-1], 1);
                comm[i-1] = 0;
                p = strstr(outcomm, ">");
                if(p!=NULL){
                    p=p+1;
                    if(*p=='>')
                        flag_out=2;
                    else
                        flag_out = 1;
                    token = strtok(outcomm, ">");
                    char* temp = (char*)malloc(strlen(token));
                    strcpy(temp, token);
                    outcomm = temp;
                    outfile = p+1;
                }

                if(flag_in!=0)
                    infd = open(infile, O_RDONLY);
                if(flag_out==1)
                    outfd = open(outfile, O_WRONLY|O_CREAT);
                else if(flag_out==2)
                    outfd = open(outfile, O_WRONLY|O_APPEND|O_CREAT);
                /////////////////////////////////////////////////////////////////////////////
                execPipe(incomm, flag_in, infd, outcomm, flag_out, outfd, comm, count-1);
                /////////////////////////////////////////////////////////////////////////////
                //printf("%s, %d, %d, %s, %d, %d, %d\n",incomm, flag_in, infd, outcomm, flag_out, outfd, count-1);
            }
            else{
                char* infile;
                char* outfile;
                char* tempcomm = (char*)malloc(strlen(command2));
                strcpy(tempcomm, command2);
                p = strstr(tempcomm, "<");
                if(p!=NULL){
                    flag_in = 1;
                    token = strtok(tempcomm, "<");
                    char* temp = (char*)malloc(strlen(token));
                    strcpy(temp, token);
                    tempcomm = temp;
                    infile = p+1;
                    infd = open(infile, O_RDONLY);
                    execPipe(tempcomm, flag_in, infd, NULL, 0, -1, NULL, -1);
                }
                else{
                    p = strstr(tempcomm, ">");
                    if(p!=NULL){
                        p=p+1;
                        if(*p=='>')
                            flag_out=2;
                        else
                            flag_out = 1;
                        token = strtok(tempcomm, ">");
                        char* temp = (char*) malloc(strlen(token));
                        strcpy(temp, token);
                        tempcomm = temp;
                        outfile = p+1;
                        if(flag_out==1)
                            outfd = open(outfile, O_WRONLY|O_CREAT, 0666);
                        else if(flag_out==2)
                            outfd = open(outfile, O_WRONLY|O_APPEND|O_CREAT, 0666);
                        execPipe(tempcomm, 0, 0, NULL, flag_out, outfd, NULL, -1);
                    }
                    else{
                        execPipe(tempcomm, 0, 0, NULL, 0, 0, NULL, -1);
                    }
                }
            }
        }
    }
}

void execDaemon(char *comm){            //comm is the command ex- "ls -l" or "ls" or "wc". No files redirection allowed

 
    sigset_t set;
    int pid,x1,fd;
    int pipefd[2];
    pipe(pipefd);

    if(sigemptyset(&set)==-1)
        perror("sigemptyset:");
    if(sigfillset(&set)==-1)
        perror("sigfillset:");
  
    if((x1=sigprocmask(SIG_UNBLOCK,&set,NULL))==-1){     //unblock all signals//dispositions are set to default at exec
        perror("sigprocmask :");
        exit(0);
    }
 
    if((pid=fork())<0){
        perror("1st fork: ");
        exit(0);
    }

    if(pid>0){
        close(pipefd[1]);
        char c[10];
        int t;
        while((t=read(pipefd[0],&c,10))!=0){
            printf("Daemon initialization done.PID= %s. STDIO,STDOUT ,STDERR redirected to the file daemon.log\n",c);
        }
        exit(0);
    }
    close(pipefd[0]);

    for( fd=sysconf(_SC_OPEN_MAX); fd>=0; --fd )     //close all fd's 
    {
        if(fd==pipefd[0] || fd==pipefd[1])
            continue;
        close(fd);
    }

    if(setsid()<0){
        perror("setsid failed :");
        exit(0);
    }

    if((pid=fork())<0){
        perror("2nd fork:");
        exit(0);
    }
    if(pid>0)
        exit(0);
                            //deamon
  
    umask(0);    //reset all file opeining masks

    char *str="daemon.log";   
    int nullfd;

    if((nullfd=open(str,O_RDWR | O_CREAT ,0666))==-1){
        perror("log file open:");
        exit(0);
    }   
    if(dup2(nullfd,2)==-1){
        perror("stderr dup: ");
        exit(0);
    }
    if(dup2(nullfd,0)==-1)
        perror("stdin dup: ");   
    if(dup2(nullfd,1)==-1)
        perror("stdout dup:"); 

    char *str2 = malloc(100);
    sprintf(str2,"Daemon initialization done. PID: %d SID: %d\n\n",getpid(),getsid(0));
    
    int temp=getpid();
    int l=snprintf(NULL,0,"%d",temp);
    char *ss=malloc(l+1);
    sprintf(ss,"%d",temp);

    if(write(pipefd[1],ss,l+1)==-1)
        perror("write to pipe");    
    close(pipefd[1]);

    write(nullfd,str2,100);
  
    execute(comm);

}

int main(){
    int ret_val;
    char *s2="\0";
    //parser("ls | wc");
    while(1){

        char command[100];
        CYAN;
        printf("$ ");
        WHITE;
        //gets(command);
        scanf("%[^\n]",command);
        strcat(command,s2);
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
