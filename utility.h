/**
 * Utility function to provide parsing and etc
 */
#include <iostream>
#include <string>

using namespace std;

#define REGISTER 0
#define REGISTER_SUCCESS 1
#define REGISTER_FAILURE 2
#define LOC_REQUEST 3
#define LOC_SUCCESS 4
#define LOC_FAILUTER 5
#define EXCUTE 6
#define EXECUTE_SUCCESS 7
#define EXECUTE_FAILUTER 8
#define TERMINATE 9

#define PARSE_ERROR -6

bool isArgTypeInput(int argType);
bool isArgTypeOutput(int argType) ;
unsigned int getArgType(int argType) ;
unsigned int getArgLength(int argType) ;
int checkArgType(int *argTypes);
int byteToInt( char * buffer);
char * intToByte(int input);
int getArgCount(int * argTypes);

//Encrypt and decrypt string
string encryptStringWithSize(string s);
int decryptInt(int fd);
int decryptString(int fd, char * s);
string decryptString(int fd);
