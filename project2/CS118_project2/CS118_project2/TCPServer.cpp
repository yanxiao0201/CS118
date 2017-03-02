//
//  TCPServer.cpp
//  RDT Project2
//
//  Created by Hao WU on 2/28/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#include "TCPServer.h"
TCPServer::TCPServer(int port){
    int sockid_=socket(AF_INET, SOCK_DGRAM, 0);// return the file descriptor of the new socket
    if (sockid_==-1) {
        perror("Socket Creation Failed");
        exit(1);
    }
    TCPServer::sockid=sockid_;
    memset((char*) &(this->my_address), 0, sizeof(this->my_address));
    my_address.sin_family=AF_INET;
    my_address.sin_port=htons(port);
    my_address.sin_addr.s_addr=htonl(INADDR_ANY);
    
    if (bind(sockid, (struct sockaddr*) &my_address, sizeof(my_address))==-1) {// bind socket to add port
        perror("Bind Failed");
        exit(1);
    }

    TCPServer::clienet_addrlen=sizeof(client_address);
    srand(time(NULL));
    my_seq_num=rand()%MAXSEQ;
}

void TCPServer::receiveSYN(){
    ssize_t receive_len=-1;
    while (true) {
        memset(buffer, 0, BUFFSIZE);
        receive_len=recvfrom(this->sockid, buffer, BUFFSIZE, 0, (struct sockaddr*) &client_address, &clienet_addrlen);
        if (receive_len<0) {
            perror("Receive SYN error\n");
            exit(1);
        }
        Data mydata(buffer,buffer+receive_len);
        Packet recv_pkt(mydata);
        if (recv_pkt.getSYN()) {
            printf("SYN received!\n");
            my_ack_num=recv_pkt.getSeq()+receive_len+1;
            break;
        }
        else{
            //printf("SYN garbled!\n");
            perror("SYN packet garbled\n");
            continue;
        }
    }
}

int TCPServer::sendSYNACK(){
    Packet mySYNACK;
    mySYNACK.setSYN(true);
    mySYNACK.setACK(true);
    mySYNACK.setAckNumber(my_ack_num);
    mySYNACK.setSeq(my_seq_num);
    Data loaded_SYNACK=mySYNACK.loadPacket();
    while (true) {
        if (sendto(sockid, loaded_SYNACK.data(), loaded_SYNACK.size(), 0, (struct sockaddr *) &client_address , clienet_addrlen)<0) {
            perror("SYNACK sent failed\n");
            continue;
        }
        else{
            break;
        }
    }
    return 999;
}

void TCPServer::sendFIN(){
    Packet myFIN;
    myFIN.setFIN(true);
    Data loaded_Fin=myFIN.loadPacket();
    while (true) {
        if (sendto(sockid, loaded_Fin.data(), loaded_Fin.size(), 0, (struct sockaddr *) &client_address , clienet_addrlen)<0) {
            perror("FIN sent failed\n");
            continue;
        }
        else{
            break;
        }
    }
    return;
}

void TCPServer::receiveFINACK(){
    ssize_t receive_len=-1;
    while (true) {
        memset(buffer, 0, BUFFSIZE);
        receive_len=recvfrom(this->sockid, buffer, BUFFSIZE, 0, (struct sockaddr*) &client_address, &clienet_addrlen);
        if (receive_len<0) {
            perror("Receive FINACK error\n");
            exit(1);
        }
        Data myData(buffer,buffer+receive_len);
        Packet recv_pkt(myData);
        if (recv_pkt.getFIN() && recv_pkt.getACK()) {
            printf("FINACK received!\n");
            my_ack_num=recv_pkt.getSeq()+receive_len+1;
            break;
        }
        else{
            //printf("SYN garbled!\n");
            perror("FINACK packet garbled\n");
            continue;
        }
    }
    close(this->sockid);
}



