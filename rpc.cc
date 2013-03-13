#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <sstream>
using namespace std;
//Library
#include "rpc.h"


// called by server
int rpcInit() {
	char* serverAddress = getenv ("BINDER_ADDRESS");
	int portNo = atoi(getenv("BINDER_PORT"));
	struct hostent *hp = gethostbyname(serverAddress);
	string input = "TESTING";
	struct sockaddr_in receiveSocket;

	//Create socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
//		error("ERROR opening socket");
		return -1;
	}

	//Set up server
	receiveSocket.sin_family = AF_INET;
	memcpy(&(receiveSocket.sin_addr.s_addr), hp->h_addr, hp->h_length);
	receiveSocket.sin_port = htons(portNo);

	//Connect
	if (connect(sockfd,(struct sockaddr *) &receiveSocket,sizeof(receiveSocket)) < 0){
//		error("ERROR connecting");
		return -1;
	}

			//bind
	int sendLen = input.length() + 1; //+1 for null character
	stringstream outputStream;

	outputStream<<input;
	const char * output = (outputStream.str()).c_str();
	//printf("%s\n",output);
	int n = write(sockfd,output,strlen(output));
	if (n < 0) {
//		error("ERROR writing to socket");
		return -1;
	}

			//Precompute the size of request , "SERVER: "+len
	int reciveLen = sendLen + 8;
	char buffer[reciveLen];
	bzero(buffer,reciveLen);

	n = read(sockfd,buffer,reciveLen);
	if (n < 0) {
//		error("ERROR reading from socket");
		return -1;
	}
	printf("%s\n",buffer);

	close(sockfd);

	return 0;
}

int rpcRegister(char* name, int* argTypes, skeleton f) {
	return 0;
}

int rpcCall(char* name, int* argTypes, void** args) {

	return 0;
}

int rpcExecute() {
	return 0;
}

int rpcTerminate() {
	return 0;
}


