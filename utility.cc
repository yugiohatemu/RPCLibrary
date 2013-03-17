#include "utility.h"
#include "rpc.h"
#include <unistd.h>
#include <sstream>

int byteToInt(char * buffer){
	int byte = 0;
	for(int i = 0; i< 4;i+=1){
		byte *= 16;
		if(buffer[i]>='0' && buffer[i]<='9'){
			byte+= (int)(buffer[i]-'0');
		}else{
			byte+= (int)(buffer[i]-'A'+10);
		}
	}
	return byte;
}

//DEBUG this first
char * intToByte(int input){
	char * buffer = new char[4];
	for(int i = 3;i >=0; i-=1 ){
		int b =  input% 16;
		if(b < 10){
			buffer[i] = char('0'+b);
		}else{
			buffer[i]  = char('A'+b-10);

		}
		input = input/16;
	}

	return buffer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Check if argument type is ok
bool isArgTypeInput(int argType) {
    // Cast to an unsigned int so that right shift fills with 0's and not 1's, like left shift.
    unsigned int input = (unsigned int)argType;
    return ((input >> ARG_INPUT) & 1) == 1;
}

bool isArgTypeOutput(int argType) {
    // Cast to an unsigned int so that right shift fills with 0's and not 1's, like left shift.
    unsigned int input = (unsigned int)argType;
    return ((input >> ARG_OUTPUT) & 1) == 1;
}

unsigned int getArgType(int argType) {
    // Cast to an unsigned int so that right shift fills with 0's, like left shift.
    unsigned int input = (unsigned int)argType;
    return (((input << 8) >> 24) & 0xFF);
}

unsigned int getArgLength(int argType) {
    // Cast to an unsigned int so that right shift fills with 0's, like left shift.
    unsigned int input = (unsigned int)argType;
    return input & 0xFFFF;
}
//check to see if the argType is valid, if so, return 0, overwrite argTypes with ARG, else, return -1
int checkArgType(int *argTypes) {
    int i = 0;
    int argType = 0;
//    int outPos = 0;
    for(int i = 0; argTypes[i] != 0; i++){
    	int argType = getArgType(argTypes[i]);
    	bool isValid = isArgTypeInput(argTypes[i]) || isArgTypeOutput(argTypes[i]);
    	if(argType < ARG_CHAR || argType > ARG_FLOAT || !isValid){
    		return -1 ; //
    	}else{
//    		if(isArgTypeOutput(argTypes[i])) outPos = i;
    		argTypes[i] = argType;
    	}
    }
    //swap(argTypes[0], argTypes[outPos]);
    return 0;
}


int getArgCount(int * argTypes){
	int count = 0;
	while(argTypes[count] != 0) count += 1;
	return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

string encryptStringWithSize(string s){
	stringstream buffer;
	buffer<<intToByte(s.length());
	buffer<<s;
	return buffer.str();
}

string encryptStringWithSize(char * s){
	stringstream buffer;
	string ss(s);
	buffer<<intToByte(ss.length());
	buffer<<ss;
	return buffer.str();
}

string encryptArgTypeWithSize(int * argTypes ){
	int count = getArgCount(argTypes);
	stringstream buffer;
	//buffer<<intToByte(count);
	for(int i = 0; i< count; i++){
		buffer<<argTypes[i];
	}
	return encryptStringWithSize(buffer.str());
}

//OutPos, # of Arg, ArgTypes to String
string encryptArgTypeWithSize(int * argTypes , int outPos){
	int count = getArgCount(argTypes);
	stringstream buffer;
	buffer<<intToByte(outPos);
	buffer<<intToByte(count);
	for(int i = 0; i< count; i++){
			buffer<<argTypes[i];
	}
	return buffer.str();
}

string encryptArgWithSize(int argType, void * args){
	stringstream buffer;

	switch(argType){
		case ARG_CHAR:
			buffer<<(*(char *)args);
			break;
		case ARG_SHORT:
			buffer<<(*(short *)args);
			break;
		case ARG_INT:
			buffer<<(*(int *)args);
			break;
		case ARG_LONG:
			buffer<<(*(long *)args);
			break;
		case ARG_DOUBLE:
			buffer<<(*(double *)args);
			break;
		case ARG_FLOAT:
			buffer<<(*(float *)args);
			break;
		default:
			return NULL;
	}
	return encryptStringWithSize(buffer.str());
}

//00010002
string encryptArgsWithSize(int * argTypes, void **args ){ //with argTypes?
	int count = getArgCount(argTypes);
	stringstream buffer;
	for(int i = 0; i< count; i++){
		buffer<<encryptArgWithSize(argTypes[i],args[i]);
	}

	return buffer.str();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Decryption Section

int decryptInt(int fd){
	char size[4] = {};
	if(read(fd,size,4)< 0){
		return -1;
	}
	return byteToInt(size);
}

string decryptString(int fd){
	int length = decryptInt(fd);
	//	cout<<"String length"<<length<<endl;
	if(length < 0){
		return NULL;
	}
	//Add one more for end of string
	char buffer[length+1];
	bzero(buffer, length+1);
	if(read(fd, buffer, length) < 0){
		return NULL;
	}
	string output(buffer);
//	cout<<output<<endl;
	return output;

}

//1st or last to be output?
int * decryptArgType(int fd){
	string s = decryptString(fd);
	cout<<"Argtype"<<s<<endl;
	//Parse the string back to fd
	int * argType;
	bzero(argType, s.length()+1);
	for(int i = 0; i< s.length();i+=1){
		if(s[i] >= '0' && s[i] <= '6') {
			argType[i] =(int) (s[i]-'0');
		}else{
			return NULL;
		}
	}
	argType[s.length()] = 0;
	return argType;
}

void * decryptArg(int argType, int fd){
	string encryptArg = decryptString(fd);
	cout<<"Decrypt "<<encryptArg<<endl;
	void  * arg = NULL;
	if(argType == ARG_CHAR){
		char ch = encryptArg[0];
		arg = (void *)&ch;
	}else if(argType == ARG_INT ){
		int  i = atoi(encryptArg.c_str());
		arg = (void *) &arg;
	}else if(argType == ARG_SHORT){
		short  i = atoi(encryptArg.c_str());
		arg = (void *) &arg;
	}else if(argType == ARG_LONG){
		long  i = atol(encryptArg.c_str());
		arg = (void *) &arg;
	}else if(argType == ARG_DOUBLE){
		double  i = atof(encryptArg.c_str());
		arg = (void *) &arg;
	}else if(argType == ARG_FLOAT){
		float  i = atof(encryptArg.c_str());
		arg = (void *) &arg;
	}
	return arg;
}


void ** decryptArgsWithType(int * argType, int fd){

	int count = getArgCount(argType);
	cout<<"Arg count "<<count<<endl;
	//WHye??
	void ** args = (void **)malloc((count+1) * sizeof(void *));
	for(int i = 0; i< count; i++){
		args[i] = decryptArg(argType[i], fd);
	}
	return args;
}
