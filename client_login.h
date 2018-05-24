#ifndef CLIENT_LOGIN_H
#define CLIENT_LOGIN_H

int client_login(int client_socket,char*name, int cflags);
int sendWOLFIE(int client_socket,char* messageReceive);
int receiveELFLOW(int client_socket,char*messageReceive);
int sendIAMnewName(int client_socket, char* name);
int receiveAfterIAMnewName(int client_socket,char* messageReceive);
int ISNewUserNameNotTaken(char* messageReceive,char* name);

int NewUserNameNotTakenAndExist(int client_socket,char* name);
void prepare_NEWPASS_message(char* messageTosend);
int sendIAMNAME(int client_socket,char*name);
int receiveAfterIAMNAME(int client_socket, char* messageReceive);
int ISuserNameTaken(char* messageReceive);
int newUserNameTaken(int client_socket);
int ISuserNameNotExist(char*messageReceive);
int ISuserNameAuth(char* messageReceive,char* name);
int abnormalMessage();
int oldUserNameTaken(int client_socket);
int oldUserNameNotExist(int client_socket);
int oldUserNameNotTakenAndExist(int client_socket,char*name);

int oldUserNameValidPassWord(int client_socket, char*messageReceive, char*name);
int oldUserNameNotValidPassWord(int client_socket,char* messageReceive);
void prepare_PASS_message(char* messageTosend);
void prepare_message(char*message_to_send,char* message);
#endif