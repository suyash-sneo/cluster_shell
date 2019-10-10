#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


#define MAXCOM 20
#define SA struct sockaddr 


void parser(char* tempcomm,char **command,char *nodes){
	
	int status;
	command = (char*)malloc(600);
	strcpy(command, tempcomm);
	char* token, token_2;
	char* p;
	p = strstr(command, ".");
	
	if(p==NULL){
		execute(command);
	}
	else{
		//char nodes[20];
		//char *commands[20];
		p = strstr(command, "*");
		if(p!=NULL){
			token = strtok(command, ".");
			token = strtok(NULL, ".");
			nodes[0] = '*';
			command[0] = (char*)(malloc(strlen(token));
			strcpy(command[0], token);
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
			for(int j=i;j<20;j++){
				nodes[j]=-1;
			}
			//send to server
		}
	}
}


void send2server(int sockfd) 
{ 
	
	fd_set rfds;
	//struct timeval tv;
	int ret;
	char buffer[1000];
	char incomm[30];
	char nodes[20];		// mx size =20
	char **commands; //max size 600=20*30
	
	while(1){

		bzero(buffer, sizeof(buffer)); 
		bzero(commands, sizeof(commands)); 
		bzero(nodes, sizeof(nodes));
		bzero(incomm, sizeof(incomm));  
		FD_ZERO(&rfds);
		FD_SET(0,&rfds);
		FD_SET(sockfd,&rfds);

		ret=select(sockfd+1,&rfds,NULL,NULL,NULL);
		if(ret==-1)
			perror("select client: ");

		if(FD_ISSET(0,&rfds)==1){			//read from stdin
			
			readfromstdin();
			parser(incomm,nodes,commands);

			if(nodes[0]=='*'){
				
				if(write(sockfd,nodes,20)==-1){
					perror("write 1 to sockfd: ");
					exit(0);
				}
				if(write(sockfd,commands,600)==-1){
					perror("write 2 to sockfd: ");
					exit(0);
				}
				for(int i=0;i<19;i++){

					if(read(sockfd,buffer,1000)<=0){
						perror("read 1 form sockfd:");
						exit(0);
					}
					printf("node %d outputs:\n%s\n",i, buffer);				
				}
				printf("my output:\n");
				int pid,status;
				if((pid=fork())==0){
					execute((char *)(*commands));
				}
				wait(&status);
			}
			else{

				if(write(sockfd,nodes,20)==-1){
					perror("write 1 to sockfd: ");
					exit(0);
				}
				if(write(sockfd,commands,600)==-1){
					perror("write 2 to sockfd: ");
					exit(0);
				}
				if(read(sockfd,buffer,1000)<=0){
					perror("read 1 form sockfd:");
					exit(0);
				}
				printf("%s\n",buffer);
			}
		}

		else if(FD_ISSET(sockfd,&rfds)==1){		//read from socketfd
			if(read(sockfd,incomm,30)==-1){
				perror("read from sockfd 2: ");
			}
			int retval;
			if((retval = fork())!=0){
				wait(&status);
			}
			else{
				close(0);
				dup(sockfd);
				close(1);
				dup(sockfd);
				execute(incomm);
			}
		}

		else{
			printf("somethings wrong..\n");
			exit(0);
		}

		/*char buff[MAXCOM]; 
		int n; 
		for (;;) { 
			bzero(buff, sizeof(buff)); 
			printf("Enter the string : "); 
			n = 0; 
			while ((buff[n++] = getchar()) != '\n') 
				; 
			write(sockfd, buff, sizeof(buff)); 
			bzero(buff, sizeof(buff)); 
			read(sockfd, buff, sizeof(buff)); 
			printf("From Server : %s", buff); 
			if ((strncmp(buff, "exit", 4)) == 0) { 
				printf("Client Exit...\n"); 
				break; 
			} 
		} */
	}
} 

int main() 
{ 
	int sockfd, connfd; 
	struct sockaddr_in servaddr, cli; 


	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		perror("client socket:");
		exit(0); 
	} 
	printf("Socketcreated\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// IP, PORT 
	servaddr.sin_family = AF_INET; 
	inet_pton(AF_INET,serv_ip,&servaddr.sin_addr); 
	servaddr.sin_port = htons(serv_port); 

	// connect 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		perror("connect to server failed\n"); 
		exit(0); 
	} 	
	printf("Connected to Server\n"); 
	send2server(sockfd); 

	close(sockfd); 
} 
