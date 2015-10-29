#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Authors: Eric Olson and Roman Claprood
Project: Lab2 Client/Server UDP with RDT 3.0
Due Date: November 4, 2015
*/

void getFileName();

int main(int argc, char *argv[])
{
	int sock,check;
	struct sockaddr_in server,client;
	socklen_t length;
	char request[256];
	char header[256];
	char filename[50];


	if (argc < 2){
		printf("Error, please give a port!\n");
		exit(1);
	}

	sock = socket(PF_INET,SOCK_DGRAM,0);
	if(sock < 0){
		printf("Error opening socket!");
		exit(0);
	}

	server.sin_family = PF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(atoi(argv[1]));

	if(bind(sock,(struct sockaddr*)&server,sizeof(server)) < 0){
		printf("Error binding socket!\n");
		exit(0);
	}

	//Gets
	while(1){
		length = sizeof(client);
		check = recvfrom(sock,request,256,0,(struct sockaddr*)&client,&length);

	}

	return 0;
}

//Used to parse response to get file name
void getFileName(char request[],char filename[]){
	int i = 0;
	char *strArray[50];
	char *token = strtok(request, " \n\r");
	//pares the message
	while(token != NULL){
		strArray[i] = malloc(strlen(token)+1);
		strcpy(strArray[i],token);
		i++;
		token = strtok(NULL, " \n\r");
	}
	strcpy(filename,strArray[i]);
}


