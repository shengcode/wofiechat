#include<stdio.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "utility.h"
#include  "array_list.h"
#include "client.h"
#include "init_client.h"
#include "client_login.h"

      


int main(int argc, char**argv){
	char name[100],serverIP[100],serverPort[100];
	int cflags=0;
	
   if (init_client(argc,argv,name,serverIP,serverPort,&cflags)==0){
   		exit(EXIT_FAILURE);
   }
	long int converted_portNumber;
	if ( (converted_portNumber=convert_portNumber(serverPort))==-1){exit(EXIT_FAILURE);}
	printf("the port number is %ld\n",converted_portNumber);
	int client_socket;
	client_socket=socket(AF_INET,SOCK_STREAM,0);
	if (client_socket==-1){perror("failed to create the client socket");exit(EXIT_FAILURE);}
	struct sockaddr_in server_address;
	server_address.sin_family=AF_INET;
	server_address.sin_port=htons((uint16_t) converted_portNumber);
	//server_address.sin_port=htons(12000);
	//server_address.sin_addr.s_addr=inet_addr("127.0.0.1");// different way of ip_address
    if(inet_aton(serverIP, (struct in_addr *)&(server_address.sin_addr.s_addr))==0){
		perror("not a valid ip address\n");
		exit(EXIT_FAILURE);
	}	
	int connectReturn;
	if( (connectReturn=connect(client_socket,(struct sockaddr*) &server_address,sizeof(struct sockaddr)))==-1){
		perror("failed to connect to the server\n");
		return 1;
	}
	
	client_login(client_socket,name,cflags);
	// after login start chat here 
	close(client_socket);
	return 0;
}



long int convert_portNumber(char* serverPort){
	char mid;
	char* endptr=&mid;
	errno =0;
	long int returnValue=strtol(serverPort, &endptr, 10);
	//underflow and overflow or no digits 
	if( (errno==ERANGE && (returnValue==LLONG_MAX || returnValue==LLONG_MIN)) || (errno==EINVAL && returnValue==0) ){
		printf("overflow/underflow happened or No digits was seen, no conversion is performed\n");
		return -1;
	}
	if(endptr==serverPort){
		printf("no digits was seen in the port number\n");
		return -1;
	}
	if (*endptr != '\0'){        /* Not necessarily an error... */
       printf("Further characters after number: %s\n", endptr);
		return -1;
	}
	return returnValue;
}