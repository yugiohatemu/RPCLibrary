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

int main(){
	int i = 30;
	char * x = intToByte(30);
	cout<<x<<endl;
	cout<<byteToInt(x)<<endl;
	return 0;
}
