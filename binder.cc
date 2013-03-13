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
using namespace std;


void error(const char *msg){
    perror(msg);
}

//Convert byte to int, function not very correct
int byteToInt(char * buffer){
	int result = 0;
	for(int i = 0;i < 4; i+=1){
		int shift = (3-i) * 8;
		if(buffer[i]>='0' && buffer[i] <= '9'){
			result += ( (buffer[i]-'0') << shift);
		}else{
			result += ( ((buffer[i]-'A') + 11) << shift);
		}
	}
	return result;
}

//Do nothing for now
Binder::Binder(){

}

//Bind the binder, if sucess, return 0, else return error
int Binder::bindSelf(){
		//Open
	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0) {
		error("ERROR opening socket");
		return -1;
	}

	//Setup
	bzero((char *) &receiveSocket, sizeof(receiveSocket));
	receiveSocket.sin_family = AF_INET;
	receiveSocket.sin_addr.s_addr = INADDR_ANY;
	receiveSocket.sin_port = 0; //bind to next available socket

	//Bind
	if (bind(listener, (struct sockaddr *) &receiveSocket, sizeof(receiveSocket)) < 0){
		  error("ERROR on binding");
		  return -1;
	}
	//Get portNo
	socklen_t server = sizeof(receiveSocket);
	//Get host and portNo
	if (getsockname(listener, (struct sockaddr *) &receiveSocket, &server) == -1){
	    error("ERROR on getting port # and address");
	    return -1;
	}else{
		char host[256] ={};
		gethostname(host, sizeof(host));
		cout<<"BINDER_ADDRESS "<<host<<endl;
	    cout<<"BINDER_PORT "<<ntohs(receiveSocket.sin_port)<<endl;
	}
	//Listen
	if(listen(listener,5)== -1){
		error("ERROR on listening");
		return -1;
	}
	// add the listener to the master set and track the max
	FD_SET(listener, &master);
	fdmax = listener;
	return 0;
}

void Binder::bindSelect(){
	//Selection
	int i;
	while(true){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			error("ERROR on select");
		}
		for(i = 0; i <=fdmax;i++){
			if(FD_ISSET(i, &read_fds)){
				if(i == listener){
					socklen_t clilen = sizeof(sendSocket);
						//Waiting for client request
					newsockfd = accept(listener, (struct sockaddr *) &sendSocket, &clilen);
					if (newsockfd < 0) {
			  			error("ERROR on accept");
					}else{
						FD_SET(newsockfd, &master); //add to server set
						if(newsockfd > fdmax){ // keep track of the maximum
							fdmax = newsockfd;
						}
					}
				}else{
					//Read first 4 byte determine size of buffer

				}
			}
		} //end forloop
	}//end while loop
}

Binder::~Binder(){
	close(listener);
}




int main(int argc, char *argv[]){
	Binder binder;
	int i = binder.bindSelf();
	if(i < 0){
		exit(1);
	}
	binder.bindSelect();
	return 0;
}
