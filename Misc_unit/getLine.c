#include "header.h"


int getLine(char *str){

	int read=0;
	size_t len=0;
	
	read=getline(&str,&len,stdin);
	if(read==-1)
		perror("getline: ");

	str[read-1]='\0';
	//write(1,str,read);
	return read;
} 



int main(){
	char *str=NULL;
	getLine(str);
	return 0;
}