default:client.cpp Packet.h Server.cpp TCPServer.h Packet.cpp TCPServer.cpp RcvBuffer.cpp RcvBuffer.h TCPClient.cpp TCPClient.h 
	g++ -std=c++0x -pthread client.cpp Packet.cpp RcvBuffer.cpp TCPClient.cpp -o client
	g++ -std=c++0x -pthread Server.cpp OutputBuffer.cpp TCPServer.cpp Packet.cpp -o server

client:
	g++ -std=c++0x -pthread client.cpp Packet.cpp RcvBuffer.cpp TCPClient.cpp -o client

server:
	g++ -std=c++0x -pthread Server.cpp OutputBuffer.cpp TCPServer.cpp Packet.cpp -o server
clean:
	rm client server

