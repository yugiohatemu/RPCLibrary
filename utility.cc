#include "utility.h"
#include "rpc.h"
#include <iostream>
using namespace std;

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

    for(int i = 0; argTypes[i] != 0; i++){
    	int argType = getArgType(argTypes[i]);
    	bool isValid = isArgTypeInput(argTypes[i]) || isArgTypeOutput(argTypes[i]);
    	if(argType < ARG_CHAR || argType > ARG_FLOAT || !isValid){
    		return -1 ; //
    	}else{
    		argTypes[i] = argType;
    	}
    }
    return 0;
}

int getArgCount(int * argTypes){
	int count = 0;
	for(; argTypes[count] != 0; count++){

	}
	return count;
}
