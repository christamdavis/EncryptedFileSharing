all: client server
	g++ -o client client.o encrypt.o decrypt.o
	g++ -o server server.o
client: client.o encrypt.o decrypt.o
	g++ -o client client.o encrypt.o decrypt.o
server: server.o
	g++ -o server server.o
server.o: server.cpp
	g++ -c server.cpp
client.o: client.cpp
	g++ -c client.cpp
encrypt.o: encrypt.cpp
	g++ -c encrypt.cpp
decrypt.o: decrypt.cpp
	g++ -c decrypt.cpp
clean:
	rm -f client server *.o
