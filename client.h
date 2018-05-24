#ifndef CLIENT_H
#define CLIENT_H



int client_login(int client_socket,char*name,int cflags);
long int convert_portNumber(char* serverPort);



#endif