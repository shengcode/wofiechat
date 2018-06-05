#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>          
#include <stdlib.h>
#include <limits.h>
#include <sqlite3.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <error.h>
#include "server.h"
#include "init_server.h"
#include "server_login.h"
#include "array_list.h"

#define BUFSIZE 1024
Arraylist *Active_Users=NULL;



int main(int argc, char**argv){	
	int vflags;
	char portNumber[100];
	char MOTD[1000];
	char accountFile[1000];
	char buf[BUFSIZE]; /* message buffer */
	int optval; /* flag value for setsockopt */
	int n; /* message byte size */
	fd_set readfds;
	int connectcnt=0;
	
	Active_Users=malloc(sizeof(Arraylist));
	init_arraylist(Active_Users,sizeof(active_userInfor));   
	
	if(init_server( argc, argv,&vflags, portNumber, MOTD,accountFile)==0){
		exit(EXIT_FAILURE);
	}	
	long int converted_portNumber;
	if ( (converted_portNumber=convert_portNumber(portNumber))==-1){exit(EXIT_FAILURE);}
	printf("the port number is %ld\n",converted_portNumber);
	int welcomeSocket;	
	struct sockaddr_in SA;
	pthread_t tid;	
	welcomeSocket=socket(AF_INET,SOCK_STREAM,0);
	if(welcomeSocket==-1) {perror("failed to create a good socket\n"); return 1;}	
	memset((void*)&SA,0,sizeof(SA));
	const char src[20]="127.0.0.1";	
	/* so that we don't need to wait for 20 seconds */
	/*optval = 1;
	setsockopt(welcomeSocket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));*/
	SA.sin_family=AF_INET;	//initialize structure
	//SA.sin_port=htons(12000);	
	SA.sin_port=htons((uint16_t) converted_portNumber);
	inet_pton(AF_INET,src,&(SA.sin_addr.s_addr));	
	if(bind(welcomeSocket,(struct sockaddr*)&SA,sizeof(struct sockaddr_in))==-1){
		perror("failed to bind the server\n");
		return 1;
	}
	else printf("successfully bind\n");
	if(listen(welcomeSocket,100)==-1){ //max length of pending connections
		perror("failed to listen\n");
		return 1;
	}
	else
	printf("now listening\n");
	printf("server>>");
	FD_ZERO(&readfds);          /* initialize the fd set */
	FD_SET(welcomeSocket, &readfds); /* add socket fd */
	FD_SET(0, &readfds);        /* add stdin fd (0) */
	while (1){		
		if (select(welcomeSocket+1, &readfds, 0, 0, 0) < 0) {
			printf("ERROR in select");
		}
		if (FD_ISSET(0, &readfds)) {
			fgets(buf, BUFSIZE, stdin);
			serverReady(buf);
		}    
		if (FD_ISSET(welcomeSocket, &readfds)) {
			printf("new clients comming\n");
			clientReady(MOTD, accountFile,welcomeSocket,connectcnt);			
		}
	}			
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
void serverReady(char* command){
	if(strcmp(command,"/users\n")==0){
			printf("this is the /users command from stdin\n");
			
	}
	else if(strcmp(command, "/help\n")==0){
		printf("this is the /help command from stdin\n");
		
	}
	else if(strcmp(command,"/shutdown\n")==0){
		printf("this is the /shutdown command from stdin\n");
		//also need to free every element in the Active_Users.
		//TODO ...need double check 
		int i;
		for(i=1;i<=Active_Users->size;i++){
			free(getIthElement(Active_Users,i));
		}
		freeArrayList(Active_Users);
		free(Active_Users);		
	}
	else if(strcmp(command,"/accts\n")==0){
		printf("this is the /accts command from stdin\n");
		
	}
	printf("server>>");	
}

void clientReady(char* MOTD, char* accountFile, int welcomeSocket,int connectcnt){
	pthread_t tid;
	struct sockaddr_in SA;
	socklen_t socketLength=sizeof(struct sockaddr);
	int clientfd = accept(welcomeSocket, (struct sockaddr *) &SA, &socketLength);
	if (clientfd < 0) {
		printf("ERROR on accept client connection"); 
		return; 
	}
	connectcnt++;
	sqlite3 *db;
	if(setUpDatabase(db)==0) return;
	struct acceptThreadArgs actThreadArg;
	actThreadArg.communicateSocket=clientfd;	
	strcpy(actThreadArg.MOTD,MOTD);
	strcpy(actThreadArg.accountFile,accountFile);	
	
	pthread_create(&tid,NULL,thread_login,(void*)(&actThreadArg));
	pthread_setname_np(tid,"LOGIN THREAD");
	puts("connection accepted\n");
}



int setUpDatabase(sqlite3 *db){
	char *zErrMsg=0;
	int rc;
	char* sql;	
	rc=sqlite3_open_v2("userInfoDB.db",&db,SQLITE_OPEN_READWRITE,NULL); // open the database when it exists
	if(rc){
		fprintf(stderr,"There is NON existent database\n");
		rc=sqlite3_open("userInfoDB.db",&db); // create a database 
		if(rc){
			fprintf(stderr,"can't Create database: %s\n",sqlite3_errmsg(db));
			return 0;
	     } 
		else {
      		fprintf(stdout, "create New database successfully\n");
			// create table 
			if(createTable(db)==1){
				sqlite3_close(db);
				return 1;
			}
			else if(createTable(db)==0){
				sqlite3_close(db);
				return 0;
			}
   		}
	}
	fprintf(stderr,"Opened existent database successfully\n");
	sqlite3_close(db);
	return 1;
}

static int createTable_callback(void*NotUser, int argc, char**argv,char**azColName){
	int i;
	for(i=0;i<argc;i++){
		printf("%s = %s\n",azColName[i],argv[i]? argv[i]:"NULL");
		printf("\n");
		return 0;
	}
  
}

int createTable(sqlite3 *db){
	//sqlite3 *db;
	char *zErrMsg=0;
	int rc;
	char* sql;
	//SQL statement
	sql = "CREATE TABLE USER_INFO("  \
         "USER_NAME TEXT PRIMARY KEY     NOT NULL," \
		 "IS_LOGIN INT,"\
         "PASSWORD  TEXT );";
	//IS_LOGIN 1 islogin, 0 is not login
   	/* Execute SQL statement */
   	rc = sqlite3_exec(db, sql, createTable_callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr,"the program failed create table");
   		fprintf(stderr, "SQL error: %s\n", zErrMsg);
      	sqlite3_free(zErrMsg);
		return 0; //failed created table
   	} 
	fprintf(stdout, "Table created successfully\n");
	return 1;
}

