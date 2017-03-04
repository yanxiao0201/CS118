//
//  TCPServer.hpp
//  RDT Project2
//
//  Created by Hao WU on 2/28/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#ifndef TCPServer_h
#define TCPServer_h

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "OutputBuffer.h"
class TCPServer{
private:
    char buffer[BUFFSIZE];
    struct sockaddr_in my_address;
    int sockid;
    int my_seq_num;
    int my_ack_num;
    struct sockaddr_in client_address;
    socklen_t clienet_addrlen;
    OutputBuffer BigBuffer;
public:
    TCPServer(int port);
    void receiveSYN();
    int sendSYNACK();
    void receivefirstACK();
    void sendFIN();
    void receiveFINACK();
    void sendFile();
};


#endif /* TCPServer_hpp */
