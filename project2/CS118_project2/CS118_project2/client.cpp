//
//  main.cpp
//  CS118_project2
//
//  Created by Xiao Yan on 2/27/17.
//  Copyright © 2017 Xiao Yan. All rights reserved.
//

#include <iostream>
#include "Packet.h"

#include <stdio.h>
#include <string.h> 	// memset, memcpy
#include <netinet/in.h> // sockaddr_in
#include <netdb.h> 		// gethostbyname
#include <stdlib.h> 	// atoi, rand
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char * argv[]) {
    if (argc != 4){
        cout << "Usage: " << argv[0] << " <server_hostname> <server_portnumber> <filename>" << endl;
        exit(1);
    }
    
    int sockfd, portno;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    
    hostname = argv[1];
    portno = atoi(argv[2]);
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        perror("Error opening socket\n");
        exit(1);
    }
    
    server = gethostbyname(hostname);
    if (server == NULL){
        perror("Error finding the host\n");
        exit(1);
    }
    
    memset((char *)&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    memcpy((char *)&serveraddr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);

    serveraddr.sin_port = htons(portno);
    
    srand(time(NULL));
    uint16_t firstPacketNo = rand() % MAXSEQ;
    
    Packet syn_packet;
    syn_packet.setSeq(firstPacketNo);
    syn_packet.setSYN(true);
    Data syn_send = syn_packet.loadPacket();

    
    if(sendto(sockfd, syn_send.data(), syn_send.size(), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
        perror("Error sending message");
        
    }
    
    
    cout << "seqNO = " << syn_packet.getSeq() << endl;
    cout << "SYN = " << syn_packet.getSYN() << endl;
    
    
    
    
    
    return 0;
}
