#include <stdio.h>
int client_login(int client_socket,char*name, int cflags){
	//-1 disconnect -10 abnormal message from server 0 failed login 1 good 
	char messageReceive[1000];
	if(sendWOLFIE(client_socket,messageReceive)!=1) return -1;
	if(receiveELFLOW(client_socket,messageReceive)!=1) return -1;
	if (cflags==1){
		if(sendIAMnewName(client_socket,name)!=1) return -1;
		if(receiveAfterIAMnewName(client_socket,messageReceive)!=1) return -1;
		if(ISuserNameTaken(messageReceive)){
			if(oldUserNameTakenOrNotExist(client_socket)!=1) return -1;
		}
		else if(ISNewUserNameNotTaken(messageReceive,name)){
			return NewUserNameNotTakenAndExist(client_socket,name)==-1;
		
		}
	}
	else if(cflags==0){
		if (sendIAMNAME(client_socket,name)!=1) return -1;
		if (receiveAfterIAMNAME(client_socket, messageReceive)!=1) return -1;
		if(ISuserNameTaken(messageReceive)|| ISuserNameNotExist(messageReceive)){
			return (oldUserNameTakenOrNotExist(client_socket));
		}
		else if (ISuserNameAuth(messageReceive,name)){
			return oldUserNameNotTakenAndExist(client_socket,name);
		}
	}
	
}	
