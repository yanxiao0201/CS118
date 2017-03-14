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
    
    while(1){
        if(!client.is_SynAck && !client.is_close){
            
            duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
            
            if(duration > 0.5){
                //This tells that we haven't received our ACk yet for this packet
                //So send it again
                std::cout << "Sending Packet SYN Retransmission" << std::endl;
                
                //Resending packet again
                if(sendto(client.sockfd, client.syn_send.data(), client.syn_send.size(), 0, (struct sockaddr *)&client.serveraddr, sizeof(client.serveraddr)) < 0){
                    perror("Error resending SYN packet");
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
        if(!client.is_RequestAck && !client.is_close){
            
            duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
            
            if(duration > 0.5){
                //This tells that we haven't received our ACk yet for this packet
                //So send it again
                
                uint16_t ack_no = (client.SynAck.getData().size() + client.SynAck.getSeq())% MAXSEQ;
                std::cout << "Sending Request Packet AckNo = " << ack_no <<" Retransmission" << std::endl;
                
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
    is_SynAck = false;
    is_RequestAck = false;
    is_close = false;
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

void TCPClient::TCPHandshake(){
    
    syn_send = syn_generator();
    
    if(sendto(sockfd, syn_send.data(), syn_send.size(), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
        perror("Error sending SYN packet");
    }
    
    std::thread resend_syn (syn_timer,std::ref(*this));
    
    while(1){
        if ((rcv = recvfrom(sockfd, buffer, BUFFSIZE, 0, (struct sockaddr *)&serveraddr, &addrlen)) < 0){
            perror("Error receiving from pipeline");
        }
        
        Data tmp(buffer,buffer + rcv);
        Packet rcv_packet(tmp);
        
        if (rcv_packet.isACK() && rcv_packet.isSYN()){
            //is SynAck
            is_SynAck = true;
            std::cout << "Receiving Packet SeqNo = " << rcv_packet.getSeq() << std::endl;
            
            SynAck = rcv_packet;
            break;
        }
        
        if (rcv_packet.isFIN()){
            //if a FIN is received
            
            std::cout << "Receiving Packet SeqNo = " << rcv_packet.getSeq() << std::endl;
            is_close = true;
            
            Data finack_send = packet_generator(rcv_packet);
            if(sendto(sockfd, finack_send.data(), finack_send.size(), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
                perror("error sending FINACK packet");
            }
            
            break;
        }
    }
    resend_syn.join();
}


void TCPClient::Send_Request(){
    if (is_close){
        return;
    }
    
    request_send = request_generator(SynAck);
    
    if(sendto(sockfd, request_send.data(), request_send.size(), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
        perror("Error sending Request packet");
    }
    
    
    std::thread resend_request(request_timer,std::ref(*this));
    
    while(1){
        if ((rcv = recvfrom(sockfd, buffer, BUFFSIZE, 0, (struct sockaddr *)&serveraddr, &addrlen)) < 0){
            perror("Error receiving from pipeline");
        }
        Data tmp(buffer,buffer + rcv);
        Packet rcv_packet(tmp);
        
        if (rcv_packet.isACK() && !rcv_packet.isFIN()){
            //is Ack
            is_RequestAck = true;
            std::cout << "Receiving Packet SeqNo = " << rcv_packet.getSeq() << std::endl;
            RequestAck = rcv_packet;
            break;
            //return rcv_packet;
        }
        
        if (rcv_packet.isFIN()){
            std::cout << "Receiving Packet SeqNo = " << rcv_packet.getSeq() << std::endl;
            is_close = true;
            
            
            Data finack_send = packet_generator(rcv_packet);
            if(sendto(sockfd, finack_send.data(), finack_send.size(), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
                perror("error sending FINACK packet");
            }
            
            break;
        }
        
    }
    resend_request.join();
}

void TCPClient::closing(){
    
    //TODO: Do I need to send FIN as well and wait for the ACK?
    
    sleep(1);
    close(sockfd);
}

void TCPClient::recv_data(std::fstream& outfile){
    
    if (is_close){
        return;
    }
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
        std::cout << "Receiving Packet SeqNo = " << rcv_packet.getSeq() << std::endl;
        
        
        if (rcv_packet.isFIN()){
            
            Data finack_send = packet_generator(rcv_packet);
            if(sendto(sockfd, finack_send.data(), finack_send.size(), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
                perror("error sending FINACK packet");
            }
            break;
            //TODO: Do I need to check if there is unwritten package?
        }
        else {
            
            Data ack_send;
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

    std::cout << "Sending Request Packet AckNo = " << ack_no << std::endl;
    
    Data ack_send = send_packet.to_UDPData();
    return ack_send;
}


