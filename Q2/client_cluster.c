#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 

#define SA struct sockaddr 


void func(int sockfd) 
{ 
	char buff[MAX]; 
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
