#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include<string.h>
#include "utility.h"
#include "array_list.h"
#include "server.h"
#include "init_server.h"


int  init_server(int argc, char *argv[],int* vflags, char* portNumber, char*MOTD,char* accountFile){
	int hflags=0,opt;
	while ((opt = getopt(argc, argv, "hv")) != -1) {
		switch (opt) {
			case 'h':
					hflags=1;
					break;
			case 'v':
					*vflags=1;
					break;
			case'?':				
			default: 
				printf("display help\n");
				return 0;
               }
           }
	if(hflags==1){
		printf("h is shown display help\n");
	}
	if(*vflags==1){
		printf("v is show\n");
	}
	if(optind<argc && (argc-optind)>=3){
		strcpy(portNumber,argv[optind++]);
		strcpy(MOTD,argv[optind++]);
		strcpy(accountFile, argv[optind++]);
	}
	else{
		printf("expect portNumber, MOTD and accountFile in the arguments\n");
		return 0;
	}
    return 1;
}