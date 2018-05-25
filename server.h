#ifndef SERVER_H
#define SERVER_H
#include <sqlite3.h>
void * thread_accept(void* vargp);
struct acceptThreadArgs{
		int communicateSocket;
		char MOTD[100];
		char accountFile[100];
};
void clientReady(char* MOTD, char* accountFile, int welcomeSocket,int connectcnt);
void serverReady(char* command);
void * thread_login(void* vargp);

long int convert_portNumber(char* serverPort);

int setUpDatabase();
int createTable(sqlite3 *db);

#endif 
