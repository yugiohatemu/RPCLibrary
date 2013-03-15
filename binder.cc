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

					//Read first 4 byte determine size of buffer
					char buffer[4] = {};
					if(read(i,buffer,4)<0){ //error
						cout<<"ERROR"<<endl;
						close(i);
						FD_CLR(i, &master);
						continue;
					}
					int size = byteToInt(buffer);
					char * message = new char[size];

//					cout<<size<<endl;
//					char type[1] = {};
					if(read(i,message,size)<0){
						cout<<"ERROR"<<endl;
					}
					cout<<message<<endl;
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
