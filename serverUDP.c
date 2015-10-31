#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
//#include <systm.h>

/*Authors: Eric Olson and Roman Claprood
Project: Lab2 Client/Server UDP with RDT 3.0
Due Date: November 4, 2015
*/

void getAndSendFile();
int RDT = 0;

int main(int argc, char *argv[])
{
	int sock,check;
	struct sockaddr_in server,client;
	socklen_t length;
	char request[50];
	memset(request,0,sizeof(request));
	if (argc < 2){
		printf("Error, please give a port!\n");
		exit(1);
	} else if (argc == 2) {
		RDT = 0;
	} else if (argc == 3) {
		RDT = 1;
	} else {
		printf("Too many arguments\n");
		exit(1);
	}

	sock = socket(PF_INET,SOCK_DGRAM,0);
	if(sock < 0){
		printf("Error opening socket!");
		exit(1);
	}

	server.sin_family = PF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(atoi(argv[1]));

	if(bind(sock,(struct sockaddr*)&server,sizeof(server)) < 0){
		printf("Error binding socket!\n");
		exit(1);
	}
	
	//had while loop around here, but wasnt working.
	length = sizeof(client);
	check = recvfrom(sock,request,sizeof(request),0,(struct sockaddr*)&client,&length);		
	getAndSendFile(sock,request,client,length);	

	return 0;
}



//Used to find file and send file to client
void getAndSendFile(int sock,char filename[],struct sockaddr_in client,socklen_t length){

	char *packet = calloc(1050,sizeof(char));
	char header[3];
	char buffer[1001];
	char Ack[2];
	int totRetrans = 0;
	int expected = 0;
	int size,maxSeqNum,sentBytes,packetNum = 0;

	struct stat fsize;
	
	//Checks if file is available
	if(access(filename,F_OK)!= -1){
		FILE *file =fopen(filename,"rb+");
		stat(filename,&fsize);
		size = fsize.st_size;
		//find total # of packets needed for file
		maxSeqNum = size / 1000;
		
		sentBytes = 0;
		int packNum = 1;
		clock_t diff, start;
		//Sends the packets
		//sendto(sock,"packet",1000,0,(struct sockaddr*)&client,length);
		while(sentBytes < size){
			memset(packet,'\0',1050);
			memset(header,'\0',3);
			memset(buffer,'\0',1001);

			header[0] = packetNum % 2;
			header[0] = header[0] + '0';
			if(packetNum < maxSeqNum){
				header[1] = '0';
			}else{
				header[1] = '1';
			}
			strcat(packet,header);
			fread(buffer,1,1000,file);
			strcat(packet,buffer);
			sentBytes += sendto(sock,packet,1002,0,(struct sockaddr*)&client,length);
			packetNum++;

			if(RDT == 1) {
				int flag = 1;
				memset(Ack, '\0', 2);
				int x;
				//start timer
				start = clock();
				diff = clock() - start;
				int sec = diff / CLOCKS_PER_SEC;
				while (flag) {
					if(sec < 5){
						x = recvfrom(sock, Ack, sizeof(Ack), MSG_DONTWAIT, (struct sockaddr*)&client, &length);	
						if (x == 0 || x == -1) {
							diff = clock() - start;
							sec = diff / CLOCKS_PER_SEC;
						} else {
							if (Ack[0] - '0' == expected) {
								printf("Expected ACK\n");
								expected = (expected + 1) % 2;
								flag = 0;
								//stop timer;
								packNum++;
							} else {
								printf("UNEXPECTED ACK!!\n");
								totRetrans++;
								printf("Retransmitting packet %d. Total number of ", packNum); 
								printf("retransmissions thus far: %d\n", totRetrans);
								sendto(sock,packet,1002,0,(struct sockaddr*)&client,length);
								//restart timer
								start = clock();
								diff = clock() - start;
								sec = diff / CLOCKS_PER_SEC;
							}
						}
					}else{
						printf("TIMEOUT REACHED!!!\n");
						totRetrans++;
						printf("Retransmitting packet %d. Total number of ", packNum); 
						printf("retransmissions thus far: %d\n", totRetrans);
						sendto(sock,packet,1002,0,(struct sockaddr*)&client,length);
						//restart timer
						start = clock();
						diff = clock() - start;
						sec = diff / CLOCKS_PER_SEC;
					}
				}
			}
		}
		fclose(file);
		close(sock);
	} else {
		printf("\nFile Not Found\n");
	}
}
