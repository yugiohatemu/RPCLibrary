#dummy makefile for now
binder:
	g++ utility.cc binder.cc -o binder
	
client:
	g++ utility.cc rpc.cc client.cc -o client

server:
	g++ server_functions.c server_function_skels.c rpc.cc utility.cc server.cc -o server


all : binder client server				# build all executables

clean:
	rm binder client server	