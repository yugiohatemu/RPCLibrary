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
#include <errno.h>
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

	cout<<"BINDER_ADDRESS "<<BINDER_ADDRESS<<endl;
	cout<<"BINDER_PORT "<<BINDER_PORT<<endl;
	binderFD = socket(AF_INET, SOCK_STREAM, 0);
	if (binderFD < 0){
		cout<<"ERROR binder socket"<<endl;
		return -1;
	}

	//Set up binder
	binderSocket.sin_family = AF_INET;
	memcpy(&(binderSocket.sin_addr.s_addr), binderHE->h_addr, binderHE->h_length);
	binderSocket.sin_port = htons(BINDER_PORT);

	//Connect
	if (connect(binderFD,(struct sockaddr *) &binderSocket,sizeof(binderSocket)) < 0){
		cout<<"ERROR connecting"<<endl;
		return -1;
	}

	return 0;
}



int initServerSocket(){
	//Set up server
	serverFD = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFD < 0) {
		cout<<"ERROR opening socket"<<endl;
		return -1;
	}

	//Setup
	bzero((char *) &serverSocket, sizeof(serverSocket));
	serverSocket.sin_family = AF_INET;
	serverSocket.sin_addr.s_addr = INADDR_ANY;
	serverSocket.sin_port = 0; //bind to next available socket

	//Bind
	if (bind(serverFD, (struct sockaddr *) &serverSocket, sizeof(serverSocket)) < 0){
		cout<<"ERROR on binding"<<endl;
		return -1;
	}
	//Get portNo
	socklen_t server = sizeof(serverSocket);

	if (getsockname(serverFD, (struct sockaddr *) &serverSocket, &server) == -1){
		cout<<"ERROR on getting port # and address"<<endl;
		return -1;
	}else{
		char host[256] ={};
		gethostname(host, sizeof(host));
		SERVER_ADDRESS = host;
		SERVER_PORT = ntohs(serverSocket.sin_port);
		cout<<"SERVER_ADDRESS "<<SERVER_ADDRESS<<endl;
		cout<<"SERVER_PORT "<<SERVER_PORT<<endl;

	}
	return 0;
}

int initOneTimeSocket(string address, int port, string message){
	struct hostent *hp = gethostbyname(address.c_str());
	struct sockaddr_in oneSocket;

	//Create socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		cout<<"ERROR opening socket";
		return -1;
	}
	//Set up server
	bzero((char *) &oneSocket, sizeof(oneSocket));
	oneSocket.sin_family = AF_INET;
	memcpy(&(oneSocket.sin_addr.s_addr), hp->h_addr, hp->h_length);
	oneSocket.sin_port = htons(port);

	//Connect
	if (connect(sockfd,(struct sockaddr *) &oneSocket,sizeof(oneSocket))< 0){
		cout<<"ERROR "<<strerror(errno)<<endl;
		return -1;
	}

	//Send message and wait for reply
	if(write(sockfd,message.c_str(),message.length()) < 0){
		cout<<"ERROR Write to Client"<<endl;
		return -1;
	}
	//Read from server
	int length = decryptInt(sockfd);
	int type = decryptInt(sockfd);
	if(length <= 0 || type < 0){
		cout<<"Type Error"<<endl;
		//return -1;
	}
	if(type == EXECUTE_SUCCESS){
		cout<<"Client Excute Sucee"<<endl;
	}
	close(sockfd);
	return 0;
}

int sendRegisterMessage(char * name, int * argTypes){
	stringstream buffer;
	//length register ip port namd , undone argType
	buffer<<intToByte(REGISTER);
	buffer<<encryptStringWithSize(SERVER_ADDRESS);
	buffer<<intToByte(SERVER_PORT);
	buffer<<encryptStringWithSize(name);
	string output = encryptStringWithSize(buffer.str());
//	cout<<output<<endl;
	if(write(binderFD,output.c_str(), output.length()) < 0){
		return -1;
	}
	//serverAddress length,   server address, port # in byte
	//Do not care about binder reply for now

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// called by server
int rpcInit() {
	cout<<"Init server"<<endl;
	int bindError = initBinderSocket();
	cout<<"Bind binder sucess"<<endl;
	int serverError = initServerSocket();
	if(bindError< 0 || serverError < 0){
		cout<<"Init server Fail"<<endl;
		return -1;
	}
	cout<<"Init server sucess"<<endl;
	return 0;
}




int rpcRegister(char* name, int* argTypes, skeleton f) {
	//Check if argType is valid
	if(checkArgType(argTypes) < 0) return -1;
	//Send message to binder
	if(sendRegisterMessage(name, argTypes)<0){

		return -1;
	}

	string sName(name);
	skeletonMap[sName] = f;

	return 0;
}



int rpcCall(char* name, int* argTypes, void** args) {
	//check if argType is valid and ask for binder
	if(checkArgType(argTypes) < 0) return -1; //parse argument error
	if(initBinderSocket() < 0) return -1;

	stringstream buffer;
	buffer<<intToByte(4)<<intToByte(LOC_REQUEST);
	buffer<<encryptStringWithSize(name);
	string output = buffer.str();

	//8 byte
	if(write(binderFD, output.c_str(),output.length())< 0 ){
		cout<<"Error Requesting function"<<endl;
		return -1;
	}

	//Always the first 8 byte
	int length = decryptInt(binderFD);
	int type = decryptInt(binderFD);
	if(length <= 0 || type < 0){
		cout<<"Type Error"<<endl;
		return -1;
	}
//	//get server address and portNo
	if(type == LOC_SUCCESS){
		int sPort = 0;
		string sAdd= decryptString(binderFD);
		sPort = decryptInt(binderFD);
		if( sPort < 0){
			return -1;
		}
		cout<<"Server "<<sAdd<<" "<<sPort<<endl;
		stringstream ss;
		//EXECUTE NAME ARGTYPES ARGS
		ss<<intToByte(4)<<intToByte(EXCUTE);
		ss<<encryptStringWithSize(name);
		string test = ss.str();
		if(initOneTimeSocket(sAdd,sPort, test)< 0 ){
			cout<<"Error to server"<<endl;
//			return -1;
		}
	}else if(type == LOC_FAILURE){
		cout<<"Fail to Excute"<<name<<endl;
	}


	close(binderFD);
	return 0;
}

/**
 ** Handle the actual excution of skeletion
 */
int rpcExecute() {

	if(listen(serverFD,5)< 0){
			cout<<"ERROR on listen"<<endl;
			return -1;
	}
	fd_set master,read_fds;
	int fdmax,newsockfd;
	struct sockaddr_in sendSocket;
	//Listen

	FD_SET(serverFD, &master);
	fdmax = serverFD;
	cout<<"Server Excute Start"<<endl;
	int i;
	while(true){

		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			cout<<"ERROR on select";
		}

		for(i = 0; i <=fdmax;i++){
			if(FD_ISSET(i, &read_fds)){
				if(i == serverFD){
					socklen_t clilen = sizeof(sendSocket);
					newsockfd = accept(serverFD, (struct sockaddr *) &sendSocket, &clilen);
					if (newsockfd < 0) {
				  		cout<<"ERROR on accept";
					}else{
						FD_SET(newsockfd, &master); //add to server set
						if(newsockfd > fdmax){ // keep track of the maximum
							fdmax = newsockfd;
						}
					}
				}else{ //Handle request
					//Read first 4 byte determine size of buffer
					int length = decryptInt(i);
					int type = decryptInt(i);
					if(length <= 0 || type < 0){
						cout<<"Client Abort connection"<<endl;
						close(i);
						FD_CLR(i, &master);
						continue;
					}
					if(type == REGISTER_SUCCESS){
						cout<<"Register sucess"<<endl;
					}else if(type == EXCUTE){
						//Try excute
						cout<<"Get client request"<<endl;
						string funcName = decryptString(i);
						if(skeletonMap.find(funcName) != skeletonMap.end()){
							stringstream buffer;
							buffer<<intToByte(EXECUTE_SUCCESS);
							string output = encryptStringWithSize(buffer.str());
							cout<<"Send Sucess Back "<<output<<endl;
							if(write(i,output.c_str(), output.length()) < 0){
								cout<<"Write Fail"<<endl;
								continue;
							}
						}else{
							stringstream buffer;
							buffer<<intToByte(EXECUTE_FAILURE);
							int reasonCode = 15;
							buffer<<intToByte(reasonCode);
							string output = encryptStringWithSize(buffer.str());
							 //Use a random code for now
							cout<<"Send Failure Back "<<output<<endl;
							if(write(i,output.c_str(), output.length()) < 0){
								cout<<"Write Fail"<<endl;
								continue;
							}
						}


						//Send a sucess message back
					}//else error
					else if(type == TERMINATE){
						cout<<"Client Terminate Connection"<<endl;
						close(i);
						FD_CLR(i, &master);
						continue;
					}
				}
			}
		} //end forloop
	}//end while loop


	close(serverFD);
	return 0;
}

int rpcTerminate() {
	stringstream buffer;
	buffer<<intToByte(TERMINATE);
	string output = encryptStringWithSize(buffer.str());
	if(write(binderFD, output.c_str(),output.length())< 0 ){
		cout<<"Error Requesting Terminate"<<endl;
		return -1;
	}
	cout<<"Client Terminate"<<endl;
	return 0;
}


