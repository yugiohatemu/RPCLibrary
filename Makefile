CXX = g++					# compiler

OBJECTS0 = server_functions.o server_function_skels.o rpc.o  server.o	 	#compile test server
EXEC0 = server

OBJECTS1 = client.o rpc.o	#compile test client
EXEC1 = client

OBJECTS2 = binder.o  #compile binder
EXEC2 = binder

OBJECTS = ${OBJECTS0} ${OBJECTS1} ${OBJECTS2} 
EXECS = ${EXEC0} ${EXEC1} ${EXEC2}	# all executables

all : ${EXECS}					# build all executables

clean:
	rm ${EXECS} 