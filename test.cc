#include <iostream>
#include <sstream>
using namespace std;

char * intToByte(int n){
	char * buffer = new char[4];
	for(int i = 3;i >=0; i-=1 ){
		int b =  n% 16;
		if(b < 10){
			buffer[i] = char('0'+b);
		}else{
			buffer[i]  = char('A'+b-10);
		}
		n = n/16;
		cout<<buffer[i];
	}
	return buffer;
}


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

char *intToChars(int integer) {
    char *returnVal = new char[4];
    returnVal[0] = (integer >> 24) & 0xFF;
    returnVal[1] = (integer >> 16) & 0xFF;
    returnVal[2] = (integer >> 8) & 0xFF;
    returnVal[3] = integer & 0xFF;

    return returnVal;
}

int main(){
	char * s = new char[2];
	s[0] = 'a';
	s[1] = '2';
//	s[2] = '\0';
	string test(s);
	cout<<s<<endl;
//	char a1 = 'a';
//	short b1 = 100;
//	int c1 = 1000;
//	long d1 = 10000;
//	int count1 = 5;
//	long return1 = 0;
//	int argTypes1[count1 + 1];
//	void **args1;
//	args1 = (void **)malloc(count1 * sizeof(void *));
//	args1[0] = (void *)&return1;
//	cout<<(* (long *) args1[0])<<endl;
//	args1[1] = (void *)&a1;
//	cout<<(* (char *) args1[1])<<endl;
//	args1[2] = (void *)&b1;
//	cout<<(* (short *) args1[2])<<endl;
//	args1[3] = (void *)&c1;
//	cout<<(* (int *) args1[3])<<endl;
//	args1[4] = (void *)&d1;
//	cout<<(* (long *) args1[4])<<endl;
////	for(int i = 0; i< 4; i++) cout<<(* (int *) args1[i])<<endl;
//
//	double d = 123.45678;
//	void * a = &d;
//	char * str =  (char *) malloc(4 * sizeof(char));
//	bzero(str, 4);
//	for(int i = 0; i< 4;i ++) str = (char *)a;
//	void * k =str;
//	double t = *(double *)k;
//	printf("%.10lf \n", t);
	//free(str);
//	delete [] str;
	return 0;
}
