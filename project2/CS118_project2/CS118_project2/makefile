default:client.cpp Packet.h Server.cpp TCPServer.h Packet.cpp TCPServer.cpp
	g++ -std=c++0x client.cpp Packet.cpp -o client
	g++ -std=c++0x Server.cpp OutputBuffer.cpp TCPServer.cpp Packet.cpp -o server

clean:
	rm client server
