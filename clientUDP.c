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


int main(int argc, char  *argv[]){
	int i, sock, portNum, prob, connection, packCount = 0;
	struct sockaddr_in server_add;
	char request[50];

	if(argc < 4){
		printf("Invalid number of arguements! Needs to be IP address, portNumber, filename, probability");
		exit(0);
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
	//printf("%s",request);
	
	//if(write(sock, request, strlen(request), 0, (struct sockaddr*)&server_add, sizeof(server_add)) < 0){
	 	//printf("Error writing to socket\n");

	if(write(sock, request, strlen(request)) < 0){
	 	printf("Error writing to socket");
	 	exit(0);
	}

	/*Here we receive the first packet from the server and obtain the total sequence number from its
	header so that we know how many packets to be expecting. Then, for each packet received, we roll
	the dice to see if we keep it or drop it.
	*/
	char buffer[1048];
	recvfrom(sock, buffer, sizeof(buffer), 0, 0, 0); //(struct sockaddr*)&server_add, sizeof(server_add)); for responding
	int totSeq = atoi((buffer+1));
	//FILE* fp = fopen(/*argv[3]*/"copytest.txt", "w");
	char newfile[50]="copy";
	strcat(newfile,argv[3]);
	FILE* fp = fopen(newfile, "w");

	srand(time(NULL));
	int r = rand() % 100; //between 0 and 99
	if(r > prob) {
		packCount++;
		printf("Packet %d accepted.\n", buffer[0]);
		for (i = 2; i < strlen(buffer); i++) {
			putc(buffer[i], fp);
		} 
	} else {
			printf("Packet %d dropped.\n", buffer[0]);
	}
	//This is set up to write the packet contents to the file no matter if it is out of order. Can be changed. 
	for (i = 1; i < totSeq; i++) {
		recvfrom(sock, buffer, sizeof(buffer), 0, 0, 0);
		r = rand() % 100;
		if(r > prob) {
			packCount++;
			printf("Packet %d accepted.\n", buffer[0]);
			for (i = 2; i < strlen(buffer); i++) {
				putc(buffer[i], fp);
			} 
		} else {
				printf("Packet %d dropped.\n", buffer[0]);
		}
	}

	printf("Received %d out of %d packets\n", packCount, totSeq);

	fclose(fp);
	close(sock);
	return 0;
	
}