#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 

#define SA struct sockaddr 



void hnadleClient(int connfd) 
{ 
	char buff[1000]; 
	int n; 
	char nodes[20];
	char commands[20][30];

	for (;;) { 
		bzero(buff, MAX); 
		int max=0;
		fd_set rfds;

		FD_ZERO(&rfds);

		for(int i=0;i<20;i++){
			max=max<connfd[i]?connfd[i]:max;
			FD_SET(connfd[i],&rfds);
		}

		if(select(max+1,&rfds,NULL,NULL,NULL)==-1){
			perror("select in server: ");
			exit(0);
		}
		int gg;
		for(gg=0;gg<20;gg++){
			if(FD_ISSET(connfd[gg],&rfds))
				break;
		}

		if(read(connfd[gg],nodes,20)==-1){
			perror("server read 1:");
			exit(0);
		}

		if(read(connfd[gg],*connfd,600)==-1){
			perror("server read 2: ");
			exit(0);
		} 
		// print buffer which contains the client contents 
		//printf("From client: %s\t To client : ", buff); 
		

		if(nodes[0]=='*'){				// n*. case 

			char strtemp[30];
			for(int i=0;i<20;i++){
				if(i==gg)
					continue;
				if(write(connfd[i],commands[0],30)==-1){
					perror("server write *:");
					exit(0);
				}
				read(connfd[i],buff,1000);
				write(connfd[gg],buff, 1000);
			}

		}
		else{
			write(connfd[0], commands[0], 30);
			read(connfd[i], buff, 1000);
			for(int i=0; i<20 && nodes[i]!=-1; i++){
				bzero(buff, MAX);
				write(connfd[i], commands[i], 30);
				write(connfd[i], buff, strlen(buff));
				read(connfd[i], buff, 1000);
			}
			bzero(buff, MAX);
			write(nodes[gg], buff, strlen(buff));
		}

	} 
} 

// Driver function 
int main() 
{ 
	if(argc!=2){
		printf("usage ./a.out <IP>");
	}


	int sockfd, connfd[20], char *ip[20],len; 
	struct sockaddr_in servaddr, cli[20]; 

	readfromfile(ip);				//read ips from file corresponding to nodes
	// socket create and verification 

	

	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		perror("socket server failed\n"); 
		exit(0); 
	} 
	printf("Socket successfully created\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(argv[1]); 
	servaddr.sin_port = htons(12345); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		perror("server bind failed: "); 
		exit(0); 
	} 
	printf("Socket successfully binded\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		perror("Listen failed\n"); 
		exit(0); 
	} 
	
	printf("Server listening\n"); 
	len = sizeof(cli[0]); 

	for(int i=0;i<20;i++){		
		// Accept the data packet from client and verification 
		connfd[i] = accept(sockfd, (SA*)&cli[i], &len); 
		if (connfd[i] < 0) { 
			perror("server acccept failed\n"); 
			exit(0); 
		} 
		
		printf("server acccepted the client %d\n",i); 
	}

	printf("All clients connected\n");

	hnadleClient(connfd,ip); 
	close(sockfd); 
} 
