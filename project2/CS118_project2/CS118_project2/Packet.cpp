//
//  packet.cpp
//  CS118_project2
//
//  Created by Xiao Yan on 2/27/17.
//  Copyright © 2017 Xiao Yan. All rights reserved.
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

void Packet::setData(Data out_data){
    myData = out_data;
}

Data Packet::getData(){
    return myData;
}
