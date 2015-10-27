#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>


int int main(int argc, char *argv[])
{
	int sock,check;
	struct sockaddr_in server,client;
	socklen_t length;
	char response[1000];

	sock = socket(PF_INET,SOCK_DGRAM,0);
	
	while(1){
		length = sizeof(client);
		
	}

	return 0;
}