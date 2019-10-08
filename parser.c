#include <stdio.h>
#include <unistd.h>                     
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

/* Copy the parser function from here into your code
 * The functions executed are
 * execMq for message queue
 * execSm for shared memory
 * execPipe for pipes
 * The function calls are highlighted using comments above and below them
 */

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
            execPipe(incomm, flag_in, infd, outcommm, flag_out, outfd, comm, count-1);
            /////////////////////////////////////////////////////////////////////////////
           //printf("%s, %d, %d, %s, %d, %d, %d\n",incomm, flag_in, infd, outcomm, flag_out, outfd, count-1);
        }
    }
}

void main(){
    parser("hello < world ## ec, ls");
}