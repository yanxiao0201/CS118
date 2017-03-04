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
        if (recv_pkt.isSYN()) {
            my_ack_num=recv_pkt.getSeq()+1;
            std::cout<<"SYN received! My ACK is "<<my_ack_num<<std::endl;
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
    Data loaded_SYNACK=mySYNACK.to_UDPData();
    while (true) {
        if (sendto(sockid, loaded_SYNACK.data(), loaded_SYNACK.size(), 0, (struct sockaddr *) &client_address , clienet_addrlen)<0) {
            perror("SYNACK sent failed\n");
            continue;
        }
        else{
            std::cout<<"SYNACK sent! My seq number is "<<my_seq_num<<std::endl;
            break;
        }
    }
    return 999;
}


void TCPServer::receivefirstACK(){
    ssize_t receive_len=-1;
    while (true) {
        memset(buffer, 0, BUFFSIZE);
        receive_len=recvfrom(this->sockid, buffer, BUFFSIZE, 0, (struct sockaddr*) &client_address, &clienet_addrlen);
        if (receive_len<0){
            perror("Receive first ACK error\n");
            exit(1);
        }
        Data mydata(buffer,buffer+receive_len);
        Packet recv_pkt(mydata);
        if (recv_pkt.isACK()) {
            my_ack_num=recv_pkt.getSeq()+receive_len+1;
            BigBuffer=OutputBuffer(5);
            std::cout<<"First ACK received! My ACK is "<<my_ack_num<<" Start Sending File..."<<std::endl;
            break;
        }
        else{
            perror("First ACK reception failure!\n");
            continue;
        }
    }
}

void TCPServer::sendFile(){
    //Packet myPacket;
    ssize_t receive_len=-1;
    int count=4;
    while (!BigBuffer.IsEmpty()) {
        count--;
        Data ready_to_send_data=BigBuffer.retreive_last();
        std::cout<<"ready_to_send content "<<ready_to_send_data[0]<<std::endl;
        Packet ready_to_send_packet;
        ready_to_send_packet.loadData(ready_to_send_data);
        ready_to_send_packet.setSeq(my_seq_num);
        Data loaded_data=ready_to_send_packet.to_UDPData();
        while (true) {
            if (sendto(sockid, loaded_data.data(), loaded_data.size(), 0, (struct sockaddr *) &client_address , clienet_addrlen)<0) {
                perror("File sent failed\n");
                continue;
            }
            else{
                std::cout<<"Part of file sent! My seq number is "<<my_seq_num<<std::endl;
                my_seq_num+=sizeof(ready_to_send_data);
                break;
            }
        }
        while (true) {
            memset(buffer, 0, BUFFSIZE);
            receive_len=recvfrom(this->sockid, buffer, BUFFSIZE, 0, (struct sockaddr*) &client_address, &clienet_addrlen);
            if (receive_len<0){
                perror("Receive first ACK error\n");
                exit(1);
            }
            Data mydata(buffer,buffer+receive_len);
            Packet recv_pkt(mydata);
            if (recv_pkt.isACK()) {
                my_ack_num=recv_pkt.getSeq()+receive_len+1;
                std::cout<<"ACK received!"<<std::endl;
                break;
            }
            else{
                perror("ACK reception failure!\n");
                continue;
            }
        }
    }
}

void TCPServer::sendFIN(){
    Packet myFIN;
    myFIN.setFIN(true);
    Data loaded_Fin=myFIN.to_UDPData();
    while (true) {
        if (sendto(sockid, loaded_Fin.data(), loaded_Fin.size(), 0, (struct sockaddr *) &client_address , clienet_addrlen)<0) {
            perror("FIN sent failed\n");
            continue;
        }
        else{
            std::cout<<"FIN sent!"<<std::endl;
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
        if (recv_pkt.isFIN() && recv_pkt.isACK()) {
            printf("FINACK received!\n");
            my_ack_num=recv_pkt.getSeq()+1;
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





