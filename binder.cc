#include "binder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sstream>
#include <netdb.h>
#include "utility.h"
using namespace std;

ServerIdenfier::ServerIdenfier(int portNo,int fd, string ip): portNo(portNo), ip(ip), fd(fd){

}

/**
 *
 *
 *
 *
 *
 *
 */

//Do nothing for now
Binder::Binder(){

}

//Bind the binder, if sucess, return 0, else return error
int Binder::bindSelf(){
		//Open
	binder = socket(AF_INET, SOCK_STREAM, 0);
	if (binder < 0) {
//		error("ERROR opening socket");
		return -1;
	}

	//Setup
	bzero((char *) &receiveSocket, sizeof(receiveSocket));
	receiveSocket.sin_family = AF_INET;
	receiveSocket.sin_addr.s_addr = INADDR_ANY;
	receiveSocket.sin_port = 0; //bind to next available socket

	//Bind
	if (bind(binder, (struct sockaddr *) &receiveSocket, sizeof(receiveSocket)) < 0){
//		  error("ERROR on binding");
		  return -1;
	}
	//Get portNo
	socklen_t server = sizeof(receiveSocket);
	//Get host and portNo
	if (getsockname(binder, (struct sockaddr *) &receiveSocket, &server) == -1){
//	    error("ERROR on getting port # and address");
	    return -1;
	}else{
		char host[256] ={};
		gethostname(host, sizeof(host));
		cout<<"BINDER_ADDRESS "<<host<<endl;
	    cout<<"BINDER_PORT "<<ntohs(receiveSocket.sin_port)<<endl;
	}
	//Listen
	return 0;
}

int Binder::bindSelect(){
	if(listen(binder,5)== -1){
	//		error("ERROR on listening");
		return -1;
	}

	FD_SET(binder, &master);
	fdmax = binder;
	//Selection
	int i;
	while(true){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
//			error("ERROR on select");
		}
		for(i = 0; i <=fdmax;i++){
			if(FD_ISSET(i, &read_fds)){
				if(i == binder){ //waiting request
					socklen_t clilen = sizeof(sendSocket);

					//Waiting for request from client or server
					newsockfd = accept(binder, (struct sockaddr *) &sendSocket, &clilen);
					if (newsockfd < 0) {
//			  			error("ERROR on accept");
					}else{
						FD_SET(newsockfd, &master); //add to server set
						if(newsockfd > fdmax){ // keep track of the maximum
							fdmax = newsockfd;
						}
					}
				}else{ //receiving request
					//Check what request if is

					//Read first 8  byte determine what mesage it is
					int length = decryptInt(i);
					int type = decryptInt(i);
					if(length <= 0 || type < 0){
						cout<<"Server/Client Abort connection"<<endl;
						close(i);
						FD_CLR(i, &master);
						continue;
					}
					cout<<"Length and Size "<<length<<" "<<type<<endl;
					if(type == REGISTER){
						//Decrypt address and port No
//						char * test;

						serverAddress = decryptString(i);
						serverPort = decryptInt(i);
						if(serverPort < 0){
							return -1;
						}
						cout<<"Register Server "<<serverAddress<<endl;
						cout<<"Register port"<<serverPort<<endl;
						//TODO: add names later
					}else if(type == LOC_REQUEST){
						cout<<"Accept Request"<<endl;
						//sendBack serverInfo simplified version
						stringstream buffer;
						buffer<<intToByte(LOC_SUCCESS);
						buffer<<encryptStringWithSize(serverAddress);
						buffer<<intToByte(serverPort);
						string output = encryptStringWithSize(buffer.str());
						cout<<"Send Rquest Back "<<output<<endl;

						if(write(i,output.c_str(), output.length()) < 0){
								cout<<"Write Fail"<<endl;
								return -1;
						}
					}
				}
			}
		} //end forloop
	}//end while loop
	return 0;
}

Binder::~Binder(){
	close(binder);
}

//////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[]){
	Binder binder;

	if(binder.bindSelf() < 0){
		exit(1);
	}
	cout<<"Binder Init Sucess"<<endl;
	if(binder.bindSelect()< 0){
		cout<<"Select fail"<<endl;
	}
	cout<<"Binder terminate"<<endl;
	return 0;
}
