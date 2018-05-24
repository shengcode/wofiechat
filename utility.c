#define _GNU_SOURCE   
#include<stdio.h> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "server_login.h"
#include "array_list.h"
#include "utility.h"


int readCharacter(int fd, char* serveBuf){
	// -1 false, 1 true
	char buf[1];	
	char endChars[5]="\r\n\r\n";
	int endProgress=0;
	Arraylist readArray;
	init_arraylist(&readArray, sizeof(char));
	while(1){
		buf[0]='0';
		int readReturn=-2;
		readReturn=read(fd, buf, 1);
		if(readReturn<0){
			perror("failed to read message");
			freeArrayList(&readArray); // if you did not read anything free the arraylist
			return -1;
		}
		/*else if(readReturn==0){
			continue;
		}*/
		else if(readReturn==1){			
			append(&readArray, (void*)&buf);
			if(buf[0]==endChars[endProgress]) {
				endProgress=endProgress+1; 
			}
			else{
				endProgress=0;
			}			
			if(endProgress==4) {
				char null_ter='\0'; //null terminate the received message 
				append(&readArray, &null_ter);
				strcpy(serveBuf,(char*)readArray.data);	
				freeArrayList(&readArray);
				return 1;
			}
		}
		
	}

}