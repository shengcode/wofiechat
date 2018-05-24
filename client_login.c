#include<stdio.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "client.h"
#include "init_client.h"
#include "client_login.h"
#include "utility.h"


int client_login(int client_socket,char*name, int cflags){
	//-1 disconnect -10 abnormal message from server 0 failed login 1 good 
	char messageReceive[1000];
	if(sendWOLFIE(client_socket,messageReceive)!=1) return -1;
	if(receiveELFLOW(client_socket,messageReceive)!=1) return -1;
	if (cflags==1){
		if(sendIAMnewName(client_socket,name)!=1) return -1;
		if(receiveAfterIAMnewName(client_socket,messageReceive)!=1) return -1;
		if(ISuserNameTaken(messageReceive)){
			//if(oldUserNameTakenOrNotExist(client_socket)!=1) return -1;
			if (newUserNameTaken(client_socket)!=1) return -1;
		}
		else if(ISNewUserNameNotTaken(messageReceive,name)){
			return NewUserNameNotTakenAndExist(client_socket,name);
		
		}
	}
	else if(cflags==0){
		if(sendIAMNAME(client_socket,name)!=1) return 1;
		if (receiveAfterIAMNAME(client_socket, messageReceive)!=1) return -1;
		if(ISuserNameTaken(messageReceive)){// username taken means already login
			return oldUserNameTaken(client_socket);
		}
		else if(ISuserNameNotExist(messageReceive)){ // username dose not exist 
			return oldUserNameNotExist(client_socket);
		}
		else if (ISuserNameAuth(messageReceive,name)){ //server send AUTHname
			return oldUserNameNotTakenAndExist(client_socket,name);
		}
	}
	
}	



int sendWOLFIE(int client_socket,char* messageReceive){
	int sendSize;
	char messageTosend[1000]="";
	char * wolfie="WOLFIE";
	prepare_message(messageTosend,wolfie);
	if((sendSize=send(client_socket,messageTosend,strlen(messageTosend),0))==-1){
		perror("failed to send message\n");
		return -1;
	}
	return readCharacter(client_socket,messageReceive);
}
int receiveELFLOW(int client_socket,char* messageReceive){
	char messageToCompare[1000]="";
	char* elflow="ELFLOW";
	prepare_message(messageToCompare,elflow);
	if(strcmp(messageReceive,messageToCompare)!=0){
		perror("not the ELFLOW message I expected\n");
		return -10;
	}
	return 1;
}

int sendIAMnewName(int client_socket, char* name){
	int sendSize;
	char messageTosend[1000]="";
	char * endNode="\r\n\r\n";	
	char* Iamnew="IAMNEW";
	strcpy(messageTosend,Iamnew);
	strcat(messageTosend,name);
	strcat(messageTosend,endNode);
	if((sendSize=send(client_socket,messageTosend,strlen(messageTosend),0))==-1  )
	{perror("failed to send message\n");return -1;}
	return 1;
}
int receiveAfterIAMnewName(int client_socket,char* messageReceive){
	return readCharacter(client_socket,messageReceive);
}


int ISNewUserNameNotTaken(char* messageReceive,char* name){
	char* hi="HINEW";
	char* endNode="\r\n\r\n";
	char messageToCompare[1000];
	strcpy(messageToCompare,hi);
	strcat(messageToCompare,name);
	strcat(messageToCompare,endNode);
	if(strcmp(messageToCompare,messageReceive)==0){
		return 1;
	}
	else
		return -1;
}


int NewUserNameNotTakenAndExist(int client_socket,char* name){
	int sendSize=-1;
	char messageTosend[100]="",messageReceive[1000]="";
	prepare_NEWPASS_message(messageTosend); // new pass 
	if((sendSize=send(client_socket,messageTosend,strlen(messageTosend),0))==-1  )
		{perror("failed to send message\n");return -1;}
	if (readCharacter(client_socket, messageReceive)==-1) return -1;
	char badPass[100]="ERR 02 BAD PASSWORD";
	strcat(badPass,"\r\n\r\n");
	char goodPass[100]="SSAPWEN";
	strcat(goodPass,"\r\n\r\n");
	if (strcmp(badPass,messageReceive)==0){
			return oldUserNameNotValidPassWord(client_socket,messageReceive);
	}
	else if (strcmp(goodPass,messageReceive)==0){
		return oldUserNameValidPassWord(client_socket,messageReceive,name);
	}
	else{
		printf("not the SSAPWEN message I expected\n");
		return -10;
	}
	return 1;
}


void prepare_NEWPASS_message(char* messageTosend){
		char passWord[1000]="0";
		char * endNode="\r\n\r\n";	
		printf("Enter the password: ");
		fgets(passWord,1000,stdin);
		int size_passWord=(int)strlen(passWord);
		passWord[size_passWord-1]='\0';
		char* pass="NEWPASS";
		strcpy(messageTosend, pass);
		strcat(messageTosend,passWord);
		strcat(messageTosend,endNode);
}

int sendIAMNAME(int client_socket,char*name){
	int sendSize, recvSize;
	char messageTosend[1000]="",messageReceive[1000]="",messageToCompare[1000]="";
	char * endNode="\r\n\r\n";	
	char* Iam="IAM";
	strcpy(messageTosend,Iam);
	strcat(messageTosend,name);
	strcat(messageTosend,endNode);
	if((sendSize=send(client_socket,messageTosend,strlen(messageTosend),0))==-1  )
	{perror("failed to send message\n");return -1;}
	return 1;
}
int receiveAfterIAMNAME(int client_socket, char* messageReceive){
	return readCharacter(client_socket, messageReceive);
}

int ISuserNameTaken(char* messageReceive){
	char* nameTaken="ERR 00 USER NAME TAKEN";
	char messageToCompare[1000];
	prepare_message(messageToCompare,nameTaken);
	if(strcmp(messageReceive,messageToCompare)==0){
		return 1;
	}
	return 0;
}
int newUserNameTaken(int client_socket){
	char messageReceive[1000]="",messageToCompare[1000]="";
	if(readCharacter(client_socket, messageReceive)==-1)
		return -1;
	char* bye="BYE";
	prepare_message(messageToCompare,bye);
	if(strcmp(messageToCompare,messageReceive)!=0){
		perror("not the BYE message I expected\n");
		return -10;
	}
	else if(strcmp(messageToCompare,messageReceive)==0){
		printf("failed login, username already taken\n");
		//disconnect the communication here
		return 0; 
	}
	return 0;
}



int ISuserNameNotExist(char*messageReceive){
	char* nameNotExist="ERR 01 USER NOT AVAILABLE";
	char messageToCompare[1000];
	prepare_message(messageToCompare,nameNotExist);
	if(strcmp(messageToCompare,messageReceive)==0){
		return 1;
	}
	return 0;
}


int ISuserNameAuth(char* messageReceive,char* name){
	char* auth="AUTH";
	char messageToCompare[1000];
	strcpy(messageToCompare,auth);
	strcat(messageToCompare,name);
	strcat(messageToCompare,"\r\n\r\n");
	//prepare_message(messageToCompare, name);
	if(strcmp(messageToCompare,messageReceive)==0){
		return 1;
	}	
	return 0;
}

int abnormalMessage(){
	printf("not the message IN  the abnormalMessage function I expected\n");
	return -10;
}




int oldUserNameTaken(int client_socket){
	char messageReceive[1000]="",messageToCompare[1000]="";
	if(readCharacter(client_socket, messageReceive)==-1)
		return -1;
	char* bye="BYE";
	prepare_message(messageToCompare,bye);
	if(strcmp(messageToCompare,messageReceive)!=0){
		perror("not the BYE message I expected\n");
		return -10;
	}
	else if(strcmp(messageToCompare,messageReceive)==0){
		printf("failed login, username already logined\n");
		//disconnect the communication here
		return 0; 
	}
	return 0;
}
int oldUserNameNotExist(int client_socket){
	char messageReceive[1000]="",messageToCompare[1000]="";
	if(readCharacter(client_socket, messageReceive)==-1)
		return -1;
	char* bye="BYE";
	prepare_message(messageToCompare,bye);
	if(strcmp(messageToCompare,messageReceive)!=0){
		perror("not the BYE message I expected\n");
		return -10;
	}
	else if(strcmp(messageToCompare,messageReceive)==0){
		printf("failed login, old username dose not exist\n");
		//disconnect the communication here
		return 0; 
	}
	return 0;


}
int oldUserNameNotTakenAndExist(int client_socket,char*name){
	int sendSize;
	char messageTosend[100]="",messageReceive[1000]="";
	prepare_PASS_message(messageTosend);
	if((sendSize=send(client_socket,messageTosend,strlen(messageTosend),0))==-1  )
		{perror("failed to send message\n");return -1;}
	if (readCharacter(client_socket, messageReceive)==-1) return -1;
	char badPass[100]="ERR 02 BAD PASSWORD";
	strcat(badPass,"\r\n\r\n");
	char goodPass[100]="PASS";
	strcat(goodPass,"\r\n\r\n");
	if (strcmp(badPass,messageReceive)==0){
			return oldUserNameNotValidPassWord(client_socket,messageReceive);
	}
	else if (strcmp(goodPass,messageReceive)==0){
		return oldUserNameValidPassWord(client_socket,messageReceive,name);
	}
	else{
		printf("not the good pass or badpass message I expected\n");
		return -10;
	}
	return 1;
}
int oldUserNameValidPassWord(int client_socket, char*messageReceive, char*name){
	if (readCharacter(client_socket, messageReceive)==-1) return -1;
	char* hi="HI";
	char * endNode="\r\n\r\n";	
	char messageToCompare[1000];
	strcpy(messageToCompare, hi);
	strcat(messageToCompare,name);
	strcat(messageToCompare,endNode);
	if(strcmp(messageToCompare,messageReceive)!=0){
		perror("not the HIname message I expected\n");
		return -10;
	}
	char* motd="MOTD";
	strcpy(messageToCompare,motd);
	if (readCharacter(client_socket, messageReceive)==-1) return -1;
	if (strncmp(messageToCompare, messageReceive, 4)!=0){
		perror("not the MOTD message I expected\n");
		return -10;
	}
	else if (strncmp(messageToCompare, messageReceive,4)==0){
		printf("successfully login in now inside the small function !!!!\n");
		return 1;  
	}
	
}
int oldUserNameNotValidPassWord(int client_socket, char* messageReceive){
	char messageToCompare[1000]="";
	if(readCharacter(client_socket, messageReceive)==-1) return -1;
	char* bye="BYE";
	prepare_message(messageToCompare,bye);
	if(strcmp(messageToCompare,messageReceive)!=0){
		perror("not the message BYE I expected\n");
		return -10;
	}
	else if(strcmp(messageToCompare,messageReceive)==0){
		printf("it it the BYE message I expected\n");
		printf("failed login,  not the correct password \n");
		//disconnect the communication here
		return 0; 
	}
	return 0;
}


void prepare_PASS_message(char* messageTosend){
		char passWord[1000]="0";
		char * endNode="\r\n\r\n";	
		printf("Enter the password: ");
		fgets(passWord,1000,stdin);
		int size_passWord=(int)strlen(passWord);
		passWord[size_passWord-1]='\0';
		char* pass="PASS";
		strcpy(messageTosend, pass);
		strcat(messageTosend,passWord);
		strcat(messageTosend,endNode);
}


void prepare_message(char*message_to_send,char* message){
	char * endNode="\r\n\r\n";	
	strcpy(message_to_send,message);
	strcat(message_to_send,endNode);
}




