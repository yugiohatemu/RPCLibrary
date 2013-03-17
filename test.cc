
#include <iostream>
#include <errno.h>
#include "utility.h"
#include "rpc.h"
#include "server_function_skels.h"
using namespace std;



void makeCBQuest(int* argTypes)
{

	int argCount = getArgCount(argTypes);
	char * res = new char[argCount * 4];


	for (int i = 0; i<argCount ; i++ )
	{
		res[4*i] = ( ((unsigned int)argTypes[i] & 0x000000FF ) );
		res[4*i+1] = ( ((unsigned int)argTypes[i] & 0x0000FF00 ) >> 8 );
		res[4*i+2] = ( ((unsigned int)argTypes[i] & 0x00FF0000 ) >> 16 );
		res[4*i+3] = ( ((unsigned int)argTypes[i] & 0xFF000000 ) >> 24 );
	}

	res[argCount*4+1] = '\0';
	for(int i = 0; i<= argCount*4;i+=1){
		cout<<res[i]<<" ";
	}

}

int main(){

		int c1 = 1000;
		int return1;
		int b1 = 100;
		int argTypes1[6];
		 argTypes1[0] = (1 << ARG_INPUT) | (ARG_CHAR << 16);
		  argTypes1[1] = (1 << ARG_OUTPUT) | (ARG_DOUBLE << 16);
		  argTypes1[2] = (1 << ARG_INPUT) | (ARG_SHORT << 16);
		  argTypes1[3] = (1 << ARG_INPUT) | (ARG_INT << 16);
		  argTypes1[4] = (1 << ARG_INPUT) | (ARG_DOUBLE << 16);
		  argTypes1[5] = 0;
		  makeCBQuest(argTypes1);


	return 0;
}

