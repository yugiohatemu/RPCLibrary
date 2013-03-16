#ifndef __BINDER__
#define __BINDER__

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include "rpc.h"
#include <map>
#include <string>
#include <set>
using namespace std;

//Need portNo, address to idenfify one server
class ServerIdenfier{
	//Ip, port #, fd that is binding
	string ip;
	int portNo;
	int fd;
public:
	ServerIdenfier(int portNo, int fd, string ip);
};



class Binder{
	int binder; //personal socket as a central database server
	struct sockaddr_in receiveSocket, sendSocket;
	fd_set master,read_fds;
	int fdmax,newsockfd;

	//keep track of servers connecting
	//keep a list of process using
	string serverAddress;
	int serverPort;
	set<string> nameSet;
public:
	Binder();
	~Binder();
	int bindSelf();
	int bindSelect();
	//Keep a mapping for process
	//The binder exists when the process exists
};

#endif
