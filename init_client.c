#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include<string.h>
#include "utility.h"
#include "array_list.h"
#include "client.h"
#include "init_client.h"
#include "client_login.h"

int  init_client(int argc, char *argv[],char* name, char*serverIP,char* serverPort, int * cflags){
	int hflags=0,vflags=0,opt;
	while ((opt = getopt(argc, argv, "hcv")) != -1) {
		switch (opt) {
			case 'h':
					hflags=1;
					break;
			case 'c':
					*cflags=1;
					break;
			case 'v':
					vflags=1;
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
	if(*cflags==1){
		
		printf("c is show\n");
	}
	if(vflags==1){
		printf("v is show\n");
	}
	if(optind<argc && (argc-optind)>=3){
		strcpy(name,argv[optind++]);
		strcpy(serverIP,argv[optind++]);
		strcpy(serverPort, argv[optind++]);
	}
	else{
		printf("expect name, serverIP and serverPort in the arguments\n");
		return 0;
	}
    return 1;
}