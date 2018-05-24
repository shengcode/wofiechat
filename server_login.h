#ifndef SERVER_LOGIN_H
#define SERVER_LOGIN_H
#include "array_list.h"
void * thread_login(void* vargp);
int receiveWOLFIE(int communicateSocket,char* messageReceive);
int sendEIFLOW(int communicateSocket);
int receiveIAMNEWorIAM(int communicateSocket,char*messageReceive);
int ISnewUser(char* messageReceive, char*name);
int ISoldUser(char* messageReceive, char*name);
int ISnameExist(char*name);
static int checkISnameExist_callBack(void*NotUser, int argc, char**argv,char**azColName);
int sendErr00Bye(int communicateSocket);
int sendHinewName(int communicateSocket, char* name);
int receiveNewPass(int communicateSocket, char* messageReceive,char*password);
int ISvalidPassword(char* password);
void saveNewUsernameAndPassword(char*name, char*password);
int sendSSAPWENandHi(int communicateSocket,char* name);
int sendMotd(int communicateSocket,char*MOTD);
int sendErr02Bye(int communicateSocket);
int sendAuth(int communicateSocket, char*name);
int receivePASS(int communicateSocket,char* messageReceive,char*password);
int IScorrectPassword(char* name, char* password);
int sendSSAPandHi(int communicateSocket,char* name);
int sendErr01Bye(int communicateSocket);

#endif