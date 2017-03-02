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
#include <unistd.h>

using namespace std;

Data packet_generator(Packet& rcv_packet);
//Data request_generator(Packet& rcv_packet, char * filename);
Data syn_generator(void);

int main(int argc, char * argv[]) {
    
    if (argc != 4){
        cout << "Usage: " << argv[0] << " <server_hostname> <server_portnumber> <filename>" << endl;
        exit(1);
    }
    
    int portno;
    char * hostname;

    hostname = argv[1];
    portno = atoi(argv[2]);
    
    int sockfd;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    socklen_t addrlen = sizeof(serveraddr);
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        perror("Error opening socket\n");
        exit(-1);
    }
    server = gethostbyname(hostname);
    if (server == NULL){
        perror("Error finding the host\n");
        exit(-1);
    }
    memset((char *)&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    memcpy((char *)&serveraddr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    
    Data syn_send = syn_generator();
    if(sendto(sockfd, syn_send.data(), syn_send.size(), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
        perror("Error sending SYN packet");
    }
    
    char buffer[BUFFSIZE];
    int rcv;

    while (1){
        rcv = recvfrom(sockfd, buffer, BUFFSIZE, 0, (struct sockaddr *)&serveraddr, &addrlen);
        if (rcv < 0){
            perror("Error receiving from pipeline");
        }
        
        Data tmp(buffer,buffer + rcv);
        Packet rcv_packet(tmp);
        Data ack_send = packet_generator(rcv_packet);
        
        if(sendto(sockfd, ack_send.data(), ack_send.size(), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
            perror("error sending ACK packet");
        }
        
        if (rcv_packet.getSYN()){
            continue;
        }
        else if (rcv_packet.getFIN()){
            break;
        }
        
        else{
            //Handle the data
            
        }
    }
    
    
    close(sockfd);
    return 0;
}

Data syn_generator(void){
    
    srand(time(NULL));
    uint16_t firstPacketNo = rand() % MAXSEQ;
    
    Packet syn_packet;
    syn_packet.setSeq(firstPacketNo);
    syn_packet.setSYN(true);
    Data syn_send = syn_packet.loadPacket();
    
    cout << "Sending SYN Packet " << firstPacketNo << endl;

    return syn_send;
}

/*
Data request_generator(Packet& rcv_packet, char * filename){
    
    Packet send_packet;
    send_packet.setSYN(true);
    
    uint16_t seq_No = rcv_packet.getACK();
    send_packet.setSeq(seq_No);
    
    send_packet.setACK(true);
    uint16_t ack_no = (rcv_packet.getData().size() + rcv_packet.getSeq())% MAXSEQ;
    send_packet.setAckNumber(ack_no);
    
    cout << "Sending ACK Packet SeqNo = " << seq_No <<" AckNo = " << ack_no << endl;
    
    Data mydata(filename, filename + sizeof(filename));
    Data ack_send = send_packet.loadPacket();
    
    return ack_send;
    
}
 */

Data packet_generator(Packet& rcv_packet){
    
    uint16_t seq_No = rcv_packet.getACK();
    Packet send_packet;
    send_packet.setSeq(seq_No);
    uint16_t ack_no = (rcv_packet.getData().size() + rcv_packet.getSeq())% MAXSEQ;
    send_packet.setACK(true);
    send_packet.setAckNumber(ack_no);
    
    if (rcv_packet.getFIN()){
        send_packet.setFIN(true);
        send_packet.setAckNumber(ack_no + 1);
        cout << "This is the FIN ACK packet" << endl;
    }
        
    cout << "Sending ACK Packet SeqNo = " << seq_No <<" AckNo = " << ack_no << endl;
    Data ack_send = send_packet.loadPacket();
    
    return ack_send;
}

