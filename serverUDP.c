#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

/*Authors: Eric Olson and Roman Claprood
Project: Lab2 Client/Server UDP with RDT 3.0
Due Date: November 4, 2015
*/

int int main(int argc, char *argv[])
{
	int sock,check;
	struct sockaddr_in server,client;
	socklen_t length;
	char response[1000];

	if (argc < 2)
	{
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


	

	return 0;
}


