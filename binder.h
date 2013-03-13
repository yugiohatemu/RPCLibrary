#ifndef __BINDER__
#define __BINDER__
//Handle the request for binding
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

class Binder{
	int listener;
	struct sockaddr_in receiveSocket, sendSocket;
	fd_set master,read_fds;
	int fdmax,newsockfd;
	//keep track of servers connecting
	//keep a list of process using
public:
	Binder();
	~Binder();
	int bindSelf();
	void bindSelect();
	//Keep a mapping for process
	//The binder exists when the process exists
};

#endif
