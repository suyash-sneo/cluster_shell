#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/errno.h>
#include<unistd.h>
#include<string.h>
  
int main() 
{ 
    char *const a[]= {"ls",0};
    execvp("ls", a);
} 