//
//  TCPClient.cpp
//  CS118_project2
//
//  Created by Xiao Yan on 3/10/17.
//  Copyright © 2017 Xiao Yan. All rights reserved.
//

#include "TCPClient.h"

void syn_timer(TCPClient& client){
    double duration;
    
    std::clock_t start;
    
    start = std::clock();
    
    //std::cout << "nohere" << std::endl;
    
    while(1){
        if(!client.is_SynAck){
            
            duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
            
            if(duration > 0.5){
                //This tells that we haven't received our ACk yet for this packet
                //So send it again
                std::cout << "Retransmit syn" << std::endl;
                
                //Resending packet again
                if(sendto(client.sockfd, client.syn_send.data(), client.syn_send.size(), 0, (struct sockaddr *)&client.serveraddr, sizeof(client.serveraddr)) < 0){
                    perror("Error sending SYN packet");
                }
                //Reseting the timer
                start = std::clock();
            }
            
        }
        
        else{
            break;
        }
        
    }
}

void request_timer(TCPClient& client){
    double duration;
    
    std::clock_t start;
    
    start = std::clock();
    
    while(1){
        if(!client.is_RequestAck){
            
            duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
            
            if(duration > 0.5){
                //This tells that we haven't received our ACk yet for this packet
                //So send it again
                std::cout << "Retransmit Request" << std::endl;
                
                //Resending packet again
                if(sendto(client.sockfd, client.request_send.data(), client.request_send.size(), 0, (struct sockaddr *)&client.serveraddr, sizeof(client.serveraddr)) < 0){
                    perror("Error sending Request packet");
                }
                //Reseting the timer
                start = std::clock();
            }
            
        }
        
        else{
            break;
        }
        
    }
    
}

TCPClient::TCPClient(){
    addrlen = sizeof(serveraddr);
}

int TCPClient::connect(char * argv[]){
    
    hostname = argv[1];
    portno = atoi(argv[2]);
    filename = argv[3];
    
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

    return sockfd;
}

Packet TCPClient::TCPHandshake(){
    
    syn_send = syn_generator();
    is_SynAck = false;
    
    if(sendto(sockfd, syn_send.data(), syn_send.size(), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
        perror("Error sending SYN packet");
    }
    
    //std::cout << "here" << std::endl;
    
    std::thread resend_syn (syn_timer,(*this));
    
    while(1){
        if ((recvfrom(sockfd, buffer, BUFFSIZE, 0, (struct sockaddr *)&serveraddr, &addrlen)) < 0){
            perror("Error receiving from pipeline");
        }
        Data tmp(buffer,buffer + rcv);
        Packet rcv_packet(tmp);
        
        if (rcv_packet.isACK() && rcv_packet.isSYN()){
            //is SynAck
            is_SynAck = true;
            
            return rcv_packet;
        }
    }
}


Packet TCPClient::Send_Request(Packet& p){
    
    request_send = request_generator(p);
    is_RequestAck = false;
    
    if(sendto(sockfd, request_send.data(), request_send.size(), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
        perror("Error sending Request packet");
    }
    
    std::thread resend_request(request_timer,(*this));
    
    while(1){
        if ((recvfrom(sockfd, buffer, BUFFSIZE, 0, (struct sockaddr *)&serveraddr, &addrlen)) < 0){
            perror("Error receiving from pipeline");
        }
        Data tmp(buffer,buffer + rcv);
        Packet rcv_packet(tmp);
        
        if (rcv_packet.isACK()){
            //is Ack
            is_RequestAck = true;
            
            return rcv_packet;
        }
    }
}

void TCPClient::closing(){
    
    close(sockfd);
}

void TCPClient::recv_data(std::fstream& outfile, Packet& RequestAck){
    //send the Ack for RequestAck, then start receive package
    Data reqAckAck = packet_generator(RequestAck);
    if(sendto(sockfd, reqAckAck.data(), reqAckAck.size(), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
        perror("error sending ACK packet");
    }

    while (1){
        
        //load received data to a packet
        rcv = recvfrom(sockfd, buffer, BUFFSIZE, 0, (struct sockaddr *)&serveraddr, &addrlen);
        if (rcv < 0){
            perror("Error receiving from pipeline");
        }
        
        Data tmp(buffer,buffer + rcv);
        Packet rcv_packet(tmp);
        
        Data ack_send;
        if (rcv_packet.isFIN()){
            
            //TODO: send FINACK
            break;
        }
        else {
            if (rcv_buffer.is_base_set() == false){
                rcv_buffer.set_rcv_base(rcv_packet.getSeq());
            }
                    
            if (rcv_buffer.insert_and_assert(rcv_packet)){
                //if return true, need to ACK, else ignore
                        
                rcv_buffer.write(outfile);
                        
                ack_send = packet_generator(rcv_packet);
                if(sendto(sockfd, ack_send.data(), ack_send.size(), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
                    perror("error sending ACK packet");
                }
            }
        }
    }

}


Data TCPClient::syn_generator(void){
    srand(time(NULL));
    uint16_t firstPacketNo = rand() % MAXSEQ;
    
    Packet syn_packet;
    syn_packet.setSeq(firstPacketNo);
    syn_packet.setSYN(true);
    Data syn_send = syn_packet.to_UDPData();
    
    std::cout << "Sending Packet SYN" << std::endl;
    
    return syn_send;
}

Data TCPClient::packet_generator(Packet& rcv_packet){
    Packet send_packet;
    
    uint16_t seq_No = rcv_packet.getAckNumber();
    send_packet.setSeq(seq_No);
    
    uint16_t ack_no = (rcv_packet.getData().size() + rcv_packet.getSeq())% MAXSEQ;
    send_packet.setACK(true);
    send_packet.setAckNumber(ack_no);
    
    std::cout << "Receiving Packet SeqNo = " << rcv_packet.getSeq() << std::endl;
    std::cout << "Sending Packet AckNo = " << ack_no;
    
    if (rcv_packet.isFIN()){
        send_packet.setFIN(true);
        std::cout << " FIN" << std::endl;
    }
    
    else{
        std::cout << std::endl;
    }
    
    Data ack_send = send_packet.to_UDPData();
    return ack_send;
}


Data TCPClient::request_generator(Packet& rcv_packet){
    Packet send_packet;
    
    uint16_t seq_No = rcv_packet.getAckNumber();
    send_packet.setSeq(seq_No);
    
    uint16_t ack_no = (rcv_packet.getData().size() + rcv_packet.getSeq())% MAXSEQ;
    send_packet.setACK(true);
    send_packet.setAckNumber(ack_no);
    
    Data temp(filename.begin(),filename.end());
    send_packet.setData(temp);
    
    std::cout << "Receiving Packet SeqNo = " << rcv_packet.getSeq() << std::endl;
    std::cout << "Sending Packet AckNo = " << ack_no << std::endl;
    
    Data ack_send = send_packet.to_UDPData();
    return ack_send;
}


