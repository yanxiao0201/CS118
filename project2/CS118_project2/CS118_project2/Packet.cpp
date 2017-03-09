//
//  Packet.cpp
//  RDT Project2
//
//  Created by Hao WU on 2/27/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#include "Packet.h"
void Packet::setHeader(uint16_t seq_number=0,uint16_t ack_number=0,bool ACK=false,bool FIN=false,bool SYN=false){
    myHeader.seq_number = seq_number;
    myHeader.ack_number = ack_number;
    myHeader.ACK = ACK;
    myHeader.FIN = FIN;
    myHeader.SYN = SYN;
}

TCPheader Packet::getHeader(){
    return myHeader;
}

void Packet::setData(Data& out_data){
    myData = out_data;
}

Data Packet::getData(){
    return myData;
}

bool Packet::isSYN(){
    return myHeader.SYN;
}

bool Packet::isACK(){
    return myHeader.ACK;
}

bool Packet::isFIN(){
    return myHeader.FIN;
}


void Packet::setACK(bool ACK){
    myHeader.ACK=ACK;
}

void Packet::setFIN(bool FIN){
    myHeader.FIN=FIN;
}

void Packet::setSYN(bool SYN){
    myHeader.SYN=SYN;
}

uint16_t Packet::getSeq(){
    return myHeader.seq_number;
}

uint16_t Packet::getAckNumber(){
    return myHeader.ack_number;
}

void Packet::setSeq(uint16_t seq_number){
    myHeader.seq_number=seq_number;
}

void Packet::setAckNumber(uint16_t ack_number){
    myHeader.ack_number=ack_number;
}

 /*int Packet::loadPacket(){
     char* pointer = buffer;
     memcpy(buffer, &myHeader.seq_number, <#size_t __n#>)
   */
    
Data Packet::to_UDPData(){
    Data result;
    uint8_t first, last;
    first = myHeader.seq_number >> 8;
    last = myHeader.seq_number;
    result.push_back(first);
    result.push_back(last);
    
    first = myHeader.ack_number >> 8;
    last = myHeader.ack_number;
    result.push_back(first);
    result.push_back(last);
    
    result.push_back(myHeader.ACK);
    result.push_back(myHeader.FIN);
    result.push_back(myHeader.SYN);
    
    result.insert(result.end(),myData.begin(),myData.end());
    return result;
}

Packet::Packet(){
    
}

void Packet::loadData(Data &ready_to_send_data){
    myData=ready_to_send_data;
}

Packet::Packet(Data& rcvData){
    myHeader.seq_number = rcvData[0] << 8 | rcvData[1];
    myHeader.ack_number = rcvData[2] << 8 | rcvData[3];
    myHeader.ACK = rcvData[4];
    myHeader.FIN = rcvData[5];
    myHeader.SYN = rcvData[6];
    if (rcvData.size() > 7){
        myData=std::vector<char>(rcvData.begin() + 7,rcvData.end());
    }
}

