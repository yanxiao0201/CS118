//
//  main.cpp
//  CS118_project2
//
//  Created by Xiao Yan on 2/27/17.
//  Copyright © 2017 Xiao Yan. All rights reserved.
//


#include "Packet.h"
#include "RcvBuffer.h"
#include "TCPClient.h"
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

using namespace std;

int main(int argc, char * argv[]) {
    
    if (argc != 4){
        cout << "Usage: " << argv[0] << " <server_hostname> <server_portnumber> <filename>" << endl;
        exit(1);
    }
    
    TCPClient client;
    
    client.connect(argv);
    
    Packet SynAck;
    SynAck = client.TCPHandshake();
    
    Packet RequestAck;
    RequestAck = client.Send_Request(SynAck);
    
    fstream outfile;
    outfile.open("received.data", ios::app|ios::out|ios::binary);

    client.recv_data(outfile, RequestAck);
    client.closing();

    outfile.close();
    
    return 0;
}








