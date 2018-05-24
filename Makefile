all: client server

client: client.o init_client.o client_login.o utility.o array_list.o 
	gcc -g client.o init_client.o client_login.o utility.o array_list.o client.h client_login.h utility.h array_list.h  -o client -l sqlite3

server: server.o init_server.o server_login.o utility.o array_list.o
	gcc -g -pthread server.o init_server.o server_login.o utility.o array_list.o server.h init_server.h server_login.h utility.h array_list.h  -o server -l sqlite3

server.o: server.c server.h
	gcc -g -c server.c server.h
init_server.o: init_server.c init_server.h
	gcc -g -c init_server.c init_server.h
server_login.o: server_login.c server_login.h
	gcc -g -c server_login.c server_login.h

client.o: client.c client.h
	gcc -g -c client.c client.h 
init_client.o: init_client.c init_client.h
	gcc -g -c init_client.c init_client.h
client_login.o:client_login.c client_login.h
	gcc -g -c client_login.c client_login.h

utility.o:utility.c utility.h
	gcc -g -c utility.c utility.h
	
array_list.o: array_list.c array_list.h
	gcc -g -c array_list.c array_list.h
	
clean:
	rm -f *.o