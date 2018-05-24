#define _GNU_SOURCE   
#include<stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "server.h"
#include "server_login.h"
#include "array_list.h"
#include "utility.h"

void * thread_login(void* vargp){
	char MOTD[100];
	char accountFile[100];
	struct acceptThreadArgs* loginThreadArg = (struct acceptThreadArgs*) vargp;
	int communicateSocket= loginThreadArg->communicateSocket; // communicateSocket is one of the argument that is passed in
	strcpy(MOTD,loginThreadArg->MOTD);
	strcpy(accountFile, loginThreadArg->accountFile);
	
	
	printf("the motd is %s\n",MOTD);
	printf("the account file is %s\n",accountFile);
	
	char messageReceive[1000]="";
	char name[100];
	char password[100];
	int continueValue=1;
	while(1){
		continueValue=receiveWOLFIE(communicateSocket, messageReceive);
		continueValue=sendEIFLOW(communicateSocket);
		continueValue=receiveIAMNEWorIAM(communicateSocket,messageReceive);
		if (ISnewUser(messageReceive, name)){
			if(!ISnameExist(name)){
				continueValue=sendHinewName(communicateSocket, name);
				continueValue=receiveNewPass(communicateSocket,messageReceive,password);
				if(ISvalidPassword(password)){
					//save username and password to database 
					saveNewUsernameAndPassword(name, password);
					continueValue=sendSSAPWENandHi(communicateSocket,name);
					continueValue=sendMotd(communicateSocket,MOTD);
					if (continueValue==1){
						// login successfully
						printf("new user login successfully\n");
						//Do whatever is needed to do 
						continue;
					}
				}
				if(!ISvalidPassword(password)){
					continueValue=sendErr02Bye(communicateSocket);
					// update database islogin information to be 0
					//disconnected
				}
			}
			if(ISnameExist(name)){
				continueValue= sendErr00Bye(communicateSocket);	
				//disconnected;
			}
		}
		else if (ISoldUser(messageReceive, name)){
			if(!ISnameExist(name)){
				//old user dose not exist 
				continueValue=sendErr01Bye(communicateSocket);
				//disconnect here
				
			}
			else if(ISnameExist(name)){
				//old user dose exist
				continueValue=sendAuth(communicateSocket, name);
				continueValue=receivePASS(communicateSocket,messageReceive,password);
				if(IScorrectPassword(name,password)){
					continueValue=sendSSAPandHi(communicateSocket,name);
					//login successfully
					// here need to update database so that  is_login is 1 
					sendMotd(communicateSocket,MOTD);
					printf("login successfully\n");
				}
				if(!IScorrectPassword(name, password)){
					continueValue=sendErr02Bye(communicateSocket);
					// no need to do the database, everything stays unchanged 
					//disconnected
				}
			}
		}
		else{
			continueValue=-10;
			perror("not New User, not Old User, not expected message\n");
			
		}
	}
}

int receiveWOLFIE(int communicateSocket,char* messageReceive){
	char messageToCompare[1000]="WOLFIE";
	char* endNode="\r\n\r\n";
	strcat(messageToCompare, endNode);
	if (readCharacter(communicateSocket,messageReceive)==0){
			perror("failed to receive message\n");
			return -1;
	}
	if(strcmp(messageToCompare, messageReceive)!=0){
		perror("not WOLFIE as I expected\n");
		return -10;
	}
	return 1;	
}

int sendEIFLOW(int communicateSocket){
	char messageToSend[1000]="ELFLOW";
	char* endNode="\r\n\r\n";
	int sendSize;
	strcat(messageToSend, endNode);
	if((sendSize=send(communicateSocket,messageToSend,strlen(messageToSend),0))==-1){
		perror("failed to send EIFLOW message\n");
		return -1;
	}
	return 1;
}

int receiveIAMNEWorIAM(int communicateSocket,char*messageReceive){
	if (readCharacter(communicateSocket,messageReceive)==0){
			perror("failed to receive IAM or IAMNEW message\n");
			return -1;
	}
	return 1;
}



int ISnewUser(char* messageReceive, char*name){
	char* messageToCompare="IAMNEW";
	if(strncmp(messageReceive,messageToCompare,6)==0){
		messageReceive[strlen(messageReceive)-4]='\0';
		strcpy(name,(messageReceive+6));
		return 1;
	}	
	return 0;	
}

int ISoldUser(char* messageReceive, char*name){
	char* messageToCompare="IAM";
	if(strncmp(messageReceive,messageToCompare,3)==0){
		messageReceive[strlen(messageReceive)-4]='\0';
		strcpy(name, (messageReceive+3));
		return 1;
	}
	return 0;
}
//also need to check if a username is already loged_in 
static int checkISnameExist_callBack(void*NotUser, int argc, char**argv,char**azColName){
	//print("");
	if( strcmp(argv[0],(char*)((int*)NotUser+1))==0 || *argv[2]==1 ){
		*(int*)NotUser=1;
	}
	return 0;
}

int ISnameExist(char*name){
	sqlite3* userInfoDB;
	char *zErrMsg=0;
	int rc;
	char* sql;
	//open database
	rc=sqlite3_open("userInfoDB.db",&userInfoDB);
	//sql statement
	sql="SELECT * FROM USER_INFO;";
	//prepare args
	void* my_args=malloc(100);
	*(int*)my_args=-1;
	strcpy( (char*)((int*)my_args+1),name);	
   	rc = sqlite3_exec(userInfoDB, sql, checkISnameExist_callBack, my_args, &zErrMsg);
	sqlite3_close(userInfoDB);
	
	if(*(int*)my_args==1) {
		free(my_args);
		return 1; // name dose exists
	}
	else if (*(int*)my_args==-1) {
		free(my_args);
		return 0; //  name does not exists
	}
}

int sendErr00Bye(int communicateSocket){
	char messageToSend[1000]="ERR 00 USER NAME TAKEN";
	char* endNode="\r\n\r\n";
	int sendSize;
	strcat(messageToSend, endNode);
	if((sendSize=send(communicateSocket,messageToSend,strlen(messageToSend),0))==-1){
		perror("failed to send ERR 00 message\n");
		return -1;
	}
	char* bye="BYE";
	strcpy(messageToSend,bye);
	strcat(messageToSend, endNode);
	if((sendSize=send(communicateSocket,messageToSend,strlen(messageToSend),0))==-1){
		perror("failed to send BYE message\n");
		return -1;
	}
	return 1;
}

int sendHinewName(int communicateSocket, char* name){
	char messageToSend[1000]="HINEW";
	int sendSize;
	strcat(messageToSend, name);
	strcat(messageToSend,"\r\n\r\n");
	if((sendSize=send(communicateSocket,messageToSend,strlen(messageToSend),0))==-1){
		perror("failed to send HINEW message\n");
		return -1;
	}
	return 1;
}

int receiveNewPass(int communicateSocket, char* messageReceive,char*password){
	char messageToCompare[1000]="NEWPASS";
	if (readCharacter(communicateSocket,messageReceive)==0){
			perror("failed to receive message\n");
			return -1;
	}
	if (strncmp(messageToCompare,messageReceive,7)==0){
		messageReceive[strlen(messageReceive)-4]='\0';
		strcpy(password,messageReceive+7);
		return 1;
	}
	perror("not the message I expected\n");
	return -10;
}
int ISvalidPassword(char* password){
	
	//return 0;
	return 1;  //now always valid password
}
void saveNewUsernameAndPassword(char*name, char*password){
	sqlite3 *db;
	char *zErrMsg=0;
	int rc;
	/*Open database */
	rc=sqlite3_open("userInfoDB.db",&db); 
   /* Create SQL statement */
	//sql="INSERT INTO USER_INFO (USER_NAME,NAME,PASSWORD) VALUES (name,password);";	
	char sql[100]="INSERT INTO USER_INFO (USER_NAME,PASSWORD,IS_LOGIN) VALUES(";
	char singleQuote[10]="\'";
	strcat(sql,singleQuote);
	strcat(sql,name);
	strcat(sql,singleQuote);
	char comma[10]=",";
	strcat(sql,comma);
	strcat(sql,singleQuote);
	strcat(sql,password);
	strcat(sql,singleQuote);
	char isLogin[10]=",1";
	char endBracket[10]=");";
	strcat(sql,endBracket);
/* Execute SQL statement */
   rc = sqlite3_exec(db, sql,NULL, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "query inserted successfully\n");
   }
   sqlite3_close(db);
  

}
int sendSSAPWENandHi(int communicateSocket,char* name){
	char messageToSend[1000]="SSAPWEN";
	strcat(messageToSend,"\r\n\r\n");
	int sendSize;
	if((sendSize=send(communicateSocket,messageToSend,strlen(messageToSend),0))==-1){
		perror("failed to send SSAPWEN message\n");
		return -1;
	}
	char* hi="HI";
	strcpy(messageToSend,hi);
	strcat(messageToSend,name);
	strcat(messageToSend,"\r\n\r\n");
	if((sendSize=send(communicateSocket,messageToSend,strlen(messageToSend),0))==-1){
		perror("failed to send hi name message\n");
		return -1;
	}
	return 1;
}
int sendMotd(int communicateSocket,char*MOTD){
	char messageToSend[1000]="MOTD";
	strcat(messageToSend,MOTD);
	strcat(messageToSend,"\r\n\r\n");
	int sendSize;
	if((sendSize=send(communicateSocket,messageToSend,strlen(messageToSend),0))==-1){
		perror("failed to send MOTD message\n");
		return -1;
	}
	return 1;
}


int sendErr02Bye(int communicateSocket){
	char messageToSend[1000]="ERR 02 BAD PASSWORD";
	char* endNode="\r\n\r\n";
	int sendSize;
	strcat(messageToSend, endNode);
	if((sendSize=send(communicateSocket,messageToSend,strlen(messageToSend),0))==-1){
		perror("failed to send ERR 02 message\n");
		return -1;
	}
	char* bye="BYE";
	strcpy(messageToSend,bye);
	strcat(messageToSend, endNode);
	if((sendSize=send(communicateSocket,messageToSend,strlen(messageToSend),0))==-1){
		perror("failed to send BYE message\n");
		return -1;
	}
	return 1;
}
int sendAuth(int communicateSocket, char*name){
	char messageToSend[1000]="AUTH";
	strcat(messageToSend,name);
	strcat(messageToSend,"\r\n\r\n");
	int sendSize;
	if((sendSize=send(communicateSocket,messageToSend,strlen(messageToSend),0))==-1){
		perror("failed to send AUTH message\n");
		return -1;
	}
	return 1;
}

int receivePASS(int communicateSocket,char* messageReceive,char*password){
	char messageToCompare[1000]="PASS";
	if (readCharacter(communicateSocket,messageReceive)==0){
			perror("failed to receive PASS message\n");
			return -1;
	}
	if(strncmp(messageToCompare,messageReceive,4)==0){
		messageReceive[strlen(messageReceive)-4]=='\0';
		strcpy(password,messageReceive);
		return 1;
	}
	perror("not the message I expeceted\n");
	return -10;
}
static int checkIScorrectPassword_callBack(void*NotUser, int argc, char**argv,char**azColName){
	if(strcmp(argv[1],(char*)((int*)NotUser+1)  )==0){
		*(int*)NotUser=1;
	}
	return 0;

}
int IScorrectPassword(char* name, char* password){
	sqlite3* userInfoDB;
	char *zErrMsg=0;
	int rc;
	
	//open database
	rc=sqlite3_open("userInfoDB.db",&userInfoDB);
	//sql statement preparation
	char sql[100]="SELECT * FROM USER_INFO WHERE USER_NAME =";
	strcat(sql, name); 
	char semicolon[]=";";
	strcat(sql,semicolon);

	//prepare args
	void* my_args=malloc(100);
	*(int*)my_args=-1;
	strcpy( (char*)((int*)my_args+1),password);	
   	rc = sqlite3_exec(userInfoDB, sql, checkIScorrectPassword_callBack, my_args, &zErrMsg);
	sqlite3_close(userInfoDB);
	
	if(*(int*)my_args==1) {
		free(my_args);
		return 1; // password matches
	}
	else if (*(int*)my_args==-1) {
		free(my_args);
		return 0; //  password dose not match
	}
	// open the file and check if the file match the password

}

int sendSSAPandHi(int communicateSocket,char* name){
	char messageToSend[1000]="SSAP";
	strcat(messageToSend,"\r\n\r\n");
	int sendSize;
	if((sendSize=send(communicateSocket,messageToSend,strlen(messageToSend),0))==-1){
		perror("failed to send SSAP message\n");
		return -1;
	}
	char* hi="HI";
	strcpy(messageToSend,hi);
	strcat(messageToSend,name);
	strcat(messageToSend,"\r\n\r\n");
	if((sendSize=send(communicateSocket,messageToSend,strlen(messageToSend),0))==-1){
		perror("failed to send hi name message\n");
		return -1;
	}
	return 1;
}

int sendErr01Bye(int communicateSocket){
	char messageToSend[1000]="ERR 01 USER NOT AVAILABLE";
	char* endNode="\r\n\r\n";
	int sendSize;
	strcat(messageToSend, endNode);
	if((sendSize=send(communicateSocket,messageToSend,strlen(messageToSend),0))==-1){
		perror("failed to send ERR 01 message\n");
		return -1;
	}
	char* bye="BYE";
	strcpy(messageToSend,bye);
	strcat(messageToSend, endNode);
	if((sendSize=send(communicateSocket,messageToSend,strlen(messageToSend),0))==-1){
		perror("failed to send BYE message\n");
		return -1;
	}
	return 1;
}