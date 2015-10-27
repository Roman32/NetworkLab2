#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>


/*
Authors: Eric Olson and Roman Claprood
Project: Lab 2 UDP server
Due Date: November 4, 2015
*/


int main(int argc, char  *argv[]){
	int sock;
	int portNum,check;
	struct sockaddr_in server_add;

	char request[256];
	//correct arguement check
	if(argc < 3){
		printf("Invalid number of arguements! Needs to be IP address/portNumber filename");
		exit(0);
	}

	portNum = atoi(argv[2]);
	sock = socket(PF_INET,SOCK_DGRAM, 0);
	if(sock < 0){
		printf("Error opening the socket!");
		exit(0);
	}

	/*sets up the connection for the server.  Different from the 
	the example in class. Used this in the minilab with TCP.
	*/
	server_add.sin_family = PF_INET;
	inet_pton(PF_INET,argv[1],&server_add.sin_addr);
	server_add.sin_port = htons(portNum);

	int connection =connect(sock,(struct sockaddr*)&server_add,sizeof(struct sockaddr_in));
	if(connection < 0){
		printf("\nError connecting!\n");
		exit(0);
	}
	//Start to build request message. Will need to add more.
	strcpy(request,"GET ");
	strcat(request,argv[2]);
	
	//Sends the request to the server.
	check = sendto(sock,request,strlen(request),0,(struct sockaddr*)&server_add,sizeof(server_add));
	if(check <0){
	 	printf("Error writing to socket");
	 	exit(0);
	}



	close(sock);
	return 0;
	
}