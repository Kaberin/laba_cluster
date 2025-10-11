all: server server_static client client_static

static: server_static client_static

nonstatic: server client

server:
	g++ .\server.cpp -o ./build/server.exe -lws2_32 -w -g

server_static:
	g++ .\server.cpp -o ./build/server_static.exe -lws2_32 -w -static

client:
	g++ .\client.cpp -o ./build/client.exe -lws2_32 -w -g

client_static:
	g++ .\client.cpp -o ./build/client_static.exe -lws2_32 -w -static

clean: 
	rmdir /S /Q build
	mkdir build