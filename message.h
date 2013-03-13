/**
 * This is for different message protocol
 */

//Length, Type, Message
#ifndef __MESSAGE__
#define __MESSAGE__
//Concatenate
class Message{
	//Enum
	enum TYPE{
		SER_BIND,
		CLI_BIND,
		SER_CLIE,
	};
	//server_identifier
	unsigned int length;
	unsigned int type;
	unsigned int argTypes;
	unsigned int port;
	int reasonCode; //failure
	char * name;
	//Constructor
	Message(char * message);
	void parseMessage();
	~Message();
	//
};

#endif
