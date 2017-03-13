//
//  TCPClient.hpp
//  CS118_project2
//
//  Created by Xiao Yan on 3/10/17.
//  Copyright © 2017 Xiao Yan. All rights reserved.
//

#ifndef TCPClient_h
#define TCPClient_h

#include "Packet.h"
#include "RcvBuffer.h"
#include <stdio.h>
#include <string.h> 	// memset, memcpy
#include <netinet/in.h> // sockaddr_in
#include <netdb.h> 		// gethostbyname
#include <stdlib.h> 	// atoi, rand
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <time.h>


class TCPClient{
private:
    
    int portno;
    char * hostname;
    std::string filename;
    
    int sockfd;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    socklen_t addrlen;
    //state state_now = WAITSYN;
    
    char buffer[BUFFSIZE];
    int rcv;
    RcvBuffer rcv_buffer;
    
    Data syn_send;
    bool is_SynAck;
    
    Data request_send;
    bool is_RequestAck;
    
    Packet SynAck;
    Packet RequestAck;
    
public:
    TCPClient();
    int connect (char * argv[]);
    void recv_data(std::fstream& outfile);
    
    void TCPHandshake();
    friend void syn_timer(TCPClient& client);
    
    void Send_Request();
    friend void request_timer(TCPClient& client);
    
    void closing();
    
    Data packet_generator(Packet& rcv_packet);
    Data request_generator(Packet& rcv_packet);
    Data syn_generator(void);

};






#endif /* TCPClient_h */
