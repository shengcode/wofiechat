#ifndef SERVER_H
#define SERVER_H
#include <sqlite3.h>
void * thread_accept(void* vargp);
struct acceptThreadArgs{
		int welcomeSocket;
		int communicateSocket;
		char MOTD[100];
		char accountFile[100];
		//sqlite3 * db;
	
};
void * thread_login(void* vargp);

long int convert_portNumber(char* serverPort);

int setUpDatabase();
int createTable(sqlite3 *db);

#endif 
