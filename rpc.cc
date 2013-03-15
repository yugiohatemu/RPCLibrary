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
#include <map>
#include <sstream>
using namespace std;
//Library
#include "rpc.h"
#include "utility.h"

//Should be static actually?
string BINDER_ADDRESS = "";
int BINDER_PORT = 0;

struct hostent *binderHE; // The binder's hostent
struct sockaddr_in binderSocket; // The binder's socket address
int binderFD; // The socket fd of the opened binder socket
map<string,skeleton> skeletonMap;

struct sockaddr_in serverSocket;
int serverFD;
string SERVER_ADDRESS = "";
int SERVER_PORT = 0;

int initBinderSocket(){
	BINDER_ADDRESS = getenv ("BINDER_ADDRESS");
	BINDER_PORT = atoi(getenv("BINDER_PORT"));
	binderHE = gethostbyname(BINDER_ADDRESS.c_str());

	binderFD = socket(AF_INET, SOCK_STREAM, 0);
	if (binderFD < 0){
	//		error("ERROR opening socket");
		return -1;
	}

	//Set up binder
	binderSocket.sin_family = AF_INET;
	memcpy(&(binderSocket.sin_addr.s_addr), binderHE->h_addr, binderHE->h_length);
	binderSocket.sin_port = htons(BINDER_PORT);

		//Connect
	if (connect(binderFD,(struct sockaddr *) &binderSocket,sizeof(binderSocket)) < 0){
		//error("ERROR connecting");
		return -1;
	}
	return 0;
}



int initServerSocket(){
	//Set up server

	serverFD = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFD < 0) {
	//	error("ERROR opening socket");
	}

	//Setup
	bzero((char *) &serverSocket, sizeof(serverSocket));
	serverSocket.sin_family = AF_INET;
	serverSocket.sin_addr.s_addr = INADDR_ANY;
	serverSocket.sin_port = 0; //bind to next available socket

	//Bind
	if (bind(serverFD, (struct sockaddr *) &serverSocket, sizeof(serverSocket)) < 0){
	//			  error("ERROR on binding");
	}
	//Get portNo
	socklen_t server = sizeof(serverSocket);

	if (getsockname(serverFD, (struct sockaddr *) &serverSocket, &server) == -1){
	//	    	error("ERROR on getting port # and address");
	}else{
		char host[256] ={};
		gethostname(host, sizeof(host));
		SERVER_ADDRESS = host;
		SERVER_PORT = ntohs(serverSocket.sin_port);
		cout<<"SERVER_ADDRESS "<<SERVER_ADDRESS<<endl;
		cout<<"SERVER_PORT "<<SERVER_PORT<<endl;

	}

	//Listen
	if(listen(serverFD,5)== -1){
		//error("ERROR on listen");
	}
	return 0;
}

int sendRegsterMessage(char * name, int * argTypes){
	stringstream buffer;
	buffer<<REGSTER;
	//server address and port #
	buffer<<intToByte(SERVER_ADDRESS.length());
	buffer<<SERVER_ADDRESS;
	buffer<<intToByte(SERVER_PORT);
	//name
	string sName(name);
	buffer<<intToByte(sName.length());
	buffer<<sName;
	//argType
	buffer<<intToByte(getArgCount(argTypes));
	buffer<<argTypes;
	//+1?? append the length at the start
	string message = buffer.str();

	stringstream endBuffer;
	endBuffer<<intToByte(message.length());
	endBuffer<<message;
//	cout<<endBuffer.str()<<endl;
	string output = endBuffer.str();
	cout<<output<<endl;

	if(write(binderFD,output.c_str(), output.length()) < 0){
		return -1;
	}
	return 0;
}
//////////////////////////////////

// called by server
int rpcInit() {
	cout<<"Init server"<<endl;
	int bindError = initBinderSocket();
	int serverError = initServerSocket();
	if(bindError< 0 || serverError < 0){
		return -1;
	}
	cout<<"Init server sucess"<<endl;
	return 0;
}




int rpcRegister(char* name, int* argTypes, skeleton f) {
	//Check if argType is valid
	if(checkArgType(argTypes) < 0) return -1;
	//Send message to binder
	if(sendRegsterMessage(name, argTypes)<0){
		cout<<"Register Fail"<<endl;
		return -1;
	}
	cout<<"Register Sucess"<<endl;

	//add it to skeleton map
	//skeletonMap
	//Parse the name and argTypes as a string for idenfication
	//and store the skeleton

	return 0;
}

//send
//	int sendLen = input.length() + 1; //+1 for null character
//
//	stringstream outputStream;
//	outputStream<<input;
//	const char * output = (outputStream.str()).c_str();
//
//
//	int n = write(sockfd,output,strlen(output));
//	if (n < 0) {
////		error("ERROR writing to socket");
//		return -1;
//	}
//
//	//Precompute the size of request , "SERVER: "+len
//	int reciveLen = sendLen + 8;
//	char buffer[reciveLen];
//	bzero(buffer,reciveLen);
//
//	n = read(sockfd,buffer,reciveLen);
//	if (n < 0) {
////		error("ERROR reading from socket");
//		return -1;
//	}
//	printf("%s\n",buffer);
//
//	close(sockfd);


int rpcCall(char* name, int* argTypes, void** args) {
	//check if argType is valid
	if(checkArgType(argTypes) < 0) return -1; //parse argument error
	//Ask if it is in the map
	//call the binder to check if there is function
	//if has, the binder will find the server and ask the server to do things, which is in rpcExcute


	return 0;
}

/**
 ** Handle the actual excution of skeletion
 */
int rpcExecute() {
	//rpcExecute
	return 0;
}

int rpcTerminate() {
	return 0;
}


