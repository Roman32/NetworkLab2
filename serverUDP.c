#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/*Authors: Eric Olson and Roman Claprood
Project: Lab2 Client/Server UDP with RDT 3.0
Due Date: November 4, 2015
*/

void getFileName();
void getAndSendFile();

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
	char contents[1001]; //one more for terminating null
	char header[3];
	char packet[1005];

	int size,maxSeqNum,sentBytes,packetNum = 0;

	struct stat fsize;
	printf("%s\n",filename);
	//Checks if file is available
	if(access(filename,F_OK)!= -1){
		FILE *file =fopen(filename,"rb");

		stat(filename,&fsize);
		size = fsize.st_size;
		//find total # of packets needed for file
		maxSeqNum = size / 1000;
		//maxSeqNum++; //get last remaining bytes.
		
		sentBytes = 0;
		//Sends the packets
		//sendto(sock,"packet",1000,0,(struct sockaddr*)&client,length);
		while(sentBytes < size){
			//memset(contents, '\0', sizeof(packet));
			//memset(header, '\0', sizeof(header));
			//memset(packet, '\0', sizeof(header));

			header[0] = packetNum % 2;
			if(packetNum < maxSeqNum){
				header[1] = 0;
			}else{
				header[1] = 1;
			}
			printf("pack number is: %d and max is: %d\n",header[0],header[1]);
			fread(contents,1,1000,file);
			printf("HERE1\n");
			strcat(packet, header);
			strcat(packet, contents);
			printf("HERE2\n");
			//printf("This is the seq num: %c\n",packet[0]);
			sentBytes += sendto(sock, packet, sizeof(packet), 0, (struct sockaddr*)&client, length);
			printf("%d\n", sentBytes);
			packetNum++;
			
		}
		fclose(file);
		close(sock);
	}else {
		printf("\nFile Not Found\n");
	}
}


