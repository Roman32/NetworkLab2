#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <time.h>
#include <stdlib.h>


/*
Authors: Eric Olson and Roman Claprood
Project: Lab 2 UDP server
Due Date: November 4, 2015
*/

void RDTSend();
void RegularSend();

int main(int argc, char  *argv[]){
	int sock, portNum, prob, connection;
	int packCount = 0;
	int endFlag = 0;
	int currPack = 0;
	int r=0;
	int expected = 0;
	int RDT = 0;
	struct sockaddr_in server_add;
	char request[50];

	if(argc < 5){
		printf("Invalid number of arguements! Needs to be IP address, portNumber, filename, probability");
		exit(0);
	} else if (argc == 5) {
		RDT = 0;
	} else if (argc == 6) {
		RDT = 1;
	} else {
		printf("Too many arguments\n");
		exit(1);
	}

	portNum = atoi(argv[2]);
	prob = atoi(argv[4]);

	sock = socket(PF_INET,SOCK_DGRAM, 0);
	if(sock < 0){
		printf("Error opening the socket!");
		exit(0);
	}

	server_add.sin_family = AF_INET;
	inet_pton(PF_INET, argv[1], &server_add.sin_addr);
	server_add.sin_port = htons(portNum);

	connection = connect(sock,(struct sockaddr*)&server_add,sizeof(struct sockaddr_in));
	if(connection < 0){
		printf("\nError connecting!\n");
		exit(0);
	}

	strcpy(request, argv[3]);
	
	//if(write(sock, request, strlen(request), 0, (struct sockaddr*)&server_add, sizeof(server_add)) < 0){
	 	//printf("Error writing to socket\n");

	if(write(sock, request, strlen(request)) < 0){
	 	printf("Error writing to socket");
	 	exit(0);
	}

	char buffer[1048];
	char Ack[2];
	//recvfrom(sock, buffer, sizeof(buffer), 0, 0, 0); //(struct sockaddr*)&server_add, sizeof(server_add)); for responding
	char newfile[50]="copy";
	strcat(newfile,argv[3]);
	FILE* fp = fopen(newfile, "w+");
	time_t t;
	srand((unsigned)time(&t));
	/*
	In the following portion of the code, we continuously receive packets from the server until we see that 
	the "final packet" flag has been set to true. It is currently set up to write the packet contents to the
	file even if they are out of order. This can be changed.
	*/
	while (endFlag == 0) {
		memset(buffer,'\0',1048);
		memset(Ack, '\0', 2);
		recvfrom(sock, buffer, 1048, 0, 0, 0);
		endFlag = buffer[1] - '0'; 
		currPack++;
		r = rand() % 100;
		if(r > prob) {
			packCount++;
			printf("Packet %d accepted.\n", currPack);
			if (RDT == 1) {
				RDTSend(buffer, &expected, Ack, &currPack, fp, &sock);
			} else if (RDT == 0) {
				RegularSend(buffer, fp);
			}
			expected = (expected + 1) % 2;
		} else {
				printf("Packet %d dropped.\n", currPack);
		}
	}

	printf("Received %d out of %d packets\n", packCount, currPack);

	fclose(fp);
	close(sock);
	return 0;
	
}

void RDTSend(char buffer[], int* expected, char Ack[], int* currPack, FILE* fp, int* sock) {
	int i;
	if (buffer[0] - '0' == *expected) {
		for (i = 2; i < strlen(buffer); i++) {
			putc(buffer[i], fp);
		} 
		Ack[0] = *expected + '0';
		//send the ack
		if(write(*sock, Ack, strlen(Ack)) < 0){
	 	printf("Error writing to socket");
	 	exit(0);
	}
	} else {
		Ack[0] = ((*expected - 1) % 2) + '0';
		//send Ack 
		if(write(*sock, Ack, strlen(Ack)) < 0){
	 	printf("Error writing to socket");
	 	exit(0);
	} 
		*currPack = *currPack - 1;
	}
}
void RegularSend(char buffer[], FILE* fp) {
	int i;
	for (i = 2; i < strlen(buffer); i++) {
				putc(buffer[i], fp);
			} 
}