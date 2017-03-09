//
//  main.cpp
//  CS118_project2
//
//  Created by Xiao Yan on 2/27/17.
//  Copyright © 2017 Xiao Yan. All rights reserved.
//


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

using namespace std;

Data packet_generator(Packet& rcv_packet);
Data request_generator(Packet& rcv_packet, string& filename);
Data syn_generator(void);

int main(int argc, char * argv[]) {
    
    if (argc != 4){
        cout << "Usage: " << argv[0] << " <server_hostname> <server_portnumber> <filename>" << endl;
        exit(1);
    }
    
    int portno;
    char * hostname;
    string filename;
    
    hostname = argv[1];
    portno = atoi(argv[2]);
    filename = argv[3];
    
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
    RcvBuffer rcv_buffer;
    fstream outfile;
    outfile.open("received.data", ios::app|ios::out|ios::binary);

    while (1){
        //load received data to a packet
        rcv = recvfrom(sockfd, buffer, BUFFSIZE, 0, (struct sockaddr *)&serveraddr, &addrlen);
        if (rcv < 0){
            perror("Error receiving from pipeline");
        }
        //cout << rcv << endl;
        
        Data tmp(buffer,buffer + rcv);
        Packet rcv_packet(tmp);
        
        //deal with send data
        Data ack_send;
        if (rcv_packet.isSYN()){
            ack_send = request_generator(rcv_packet, filename);
        }
        else{
            ack_send = packet_generator(rcv_packet);
        }
        
        if(sendto(sockfd, ack_send.data(), ack_send.size(), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
            perror("error sending ACK packet");
        }
        
        // deal with received data
        int buffsize = rcv_buffer.buffsize();
        if (rcv_packet.isFIN()){
            
            if (buffsize != 0){
                for (int i = 0; i < buffsize; i++){
                    outfile.write(rcv_buffer.getBuffer()[i].thisData.data(), rcv_buffer.getBuffer()[i].thisData.size());
                }
                rcv_buffer.clean();
            }
            break;
        }
        else{
            if (buffsize == WNDSIZE){
                //write to output file
                
                for (int i = 0; i < WNDSIZE; i++){
                    outfile.write(rcv_buffer.getBuffer()[i].thisData.data(), rcv_buffer.getBuffer()[i].thisData.size());
                }
                rcv_buffer.clean();
            }
            
            Data rcv_data = rcv_packet.getData();
            if (rcv_data.size() != 0){
                rcv_buffer.insert(rcv_packet);
            }
            
        }
    }

    outfile.close();
    close(sockfd);
    return 0;
}

Data syn_generator(void){
    srand(time(NULL));
    uint16_t firstPacketNo = rand() % MAXSEQ;
    
    Packet syn_packet;
    syn_packet.setSeq(firstPacketNo);
    syn_packet.setSYN(true);
    Data syn_send = syn_packet.to_UDPData();
    
    cout << "Sending Packet SYN" << endl;

    return syn_send;
}

Data packet_generator(Packet& rcv_packet){
    Packet send_packet;
    
    uint16_t seq_No = rcv_packet.getAckNumber();
    send_packet.setSeq(seq_No);
    
    uint16_t ack_no = (rcv_packet.getData().size() + rcv_packet.getSeq())% MAXSEQ;
    send_packet.setACK(true);
    send_packet.setAckNumber(ack_no);

    cout << "Receiving Packet SeqNo = " << rcv_packet.getSeq() << endl;
    cout << "Sending Packet AckNo = " << ack_no;
    
    if (rcv_packet.isFIN()){
        send_packet.setFIN(true);
        cout << " FIN" << endl;
    }
    
    else{
        cout << endl;
    }
    
    Data ack_send = send_packet.to_UDPData();
    return ack_send;
}

Data request_generator(Packet& rcv_packet, string& filename){
    Packet send_packet;
    
    uint16_t seq_No = rcv_packet.getAckNumber();
    send_packet.setSeq(seq_No);
    
    uint16_t ack_no = (rcv_packet.getData().size() + rcv_packet.getSeq())% MAXSEQ;
    send_packet.setACK(true);
    send_packet.setAckNumber(ack_no);
    
    Data temp(filename.begin(),filename.end());
    send_packet.setData(temp);
    
    cout << "Receiving Packet SeqNo = " << rcv_packet.getSeq() << endl;
    cout << "Sending Packet AckNo = " << ack_no << endl;
    
    Data ack_send = send_packet.to_UDPData();
    return ack_send;
}


