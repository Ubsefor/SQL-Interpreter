all:
	make server
	make client

server:
	g++ ./server/server.cpp ./server/table.cpp ./server/lexer.cpp ./server/parser.cpp -o server
	
client:
	g++ ./client/client.cpp -o client
	
