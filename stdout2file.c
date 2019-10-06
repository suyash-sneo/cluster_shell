#include "header.h"

int main(int argc, char *argv[]){

		int fp;
		//if(!strcmp(argv[2],">")){
			//printf(">\n");
			//if((fp=open(argv[2],O_CREAT | O_TRUNC | O_RDWR))==-1)
			//	perror("open: ");
		//}
		//else if(!strcmp(argv[2],">>")){
		//	printf(">>\n");
			fp=open(argv[2],O_CREAT | O_APPEND | O_RDWR);
		//}
		//else{
		//	printf("Usage error\n");
		//}*/

		int err1,err2;
		if((err1=dup2(fp,1))==-1)
			perror("dup2: ");


		if( (err2 = execlp(argv[1],argv[1],(char *)0)) == -1)
			perror("exec: ");
}