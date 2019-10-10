#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void parser(char* tempcomm){
	char * command = (char*)malloc(strlen(tempcomm));
	strcpy(command, tmepcomm);
	char* token, token_2;
	char* p;
	p = strstr(command, ".");
	if(p==NULL){
		execute(command);
	}
	else{
		char nodes[20];
		char *commands[20];
		p = strstr(command, "*");
		if(p!=NULL){
			token = strtok(command, ".");
			token = strtok(NULL, ".");
			nodes[0] = '*';
			commands[0] = char*(malloc(strlen(token));
			strcpy(commands[0], token);
			//send to server
		}
		else{
			int i=0;
			token = strtok(command, "|");
			while(token!=NULL){
				char* temp = (char*)malloc(strlen(token));
				strcpy(temp, token);
				token_2 = strtok(token, ".");
				p = strstr(temp, ".");
				nodes[i] = token_2[1];
				commands[i] = p+1;
				i++;
				token = strtok(NULL, "|");
			}
			//send to server
		}
	}
}