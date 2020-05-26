all: server.uex client.uex

server.uex:
		g++ -std=c++14 server/server.cpp server/table.cpp server/lexer.cpp server/parser.cpp -o server.uex
	
client.uex:
		g++ -std=c++14 client/client.cpp -o client.uex
	
