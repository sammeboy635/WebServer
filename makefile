main: server.o 
	g++ -std=gnu++2a -g src/main.cpp lib/server.o  -o main

server.o:
	g++ -o lib/server.o -Wall -g -c src/server.cpp



run:
	./main 
