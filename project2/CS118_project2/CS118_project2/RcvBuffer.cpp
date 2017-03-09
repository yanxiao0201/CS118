//
//  RcvBuffer.cpp
//  CS118_project2
//
//  Created by Xiao Yan on 3/5/17.
//  Copyright © 2017 Xiao Yan. All rights reserved.
//

#include "RcvBuffer.h"


int RcvBuffer::insert(Packet p){
    
    Data tmp = p.getData();
    uint16_t seqNo = p.getSeq();
    rcvseg rcv_seg;
    rcv_seg.seqNo = seqNo;
    rcv_seg.thisData = tmp;
    winBuff.push_back(rcv_seg);
    
    return 0;
}

std::vector<rcvseg> RcvBuffer::getBuffer(){
    
    return winBuff;
    
}

int RcvBuffer::buffsize(){
    return winBuff.size();
}

void RcvBuffer::clean(){
    winBuff.clear();
}

bool RcvBuffer::isACKed_i(int i){
    return winBuff[i].ACKed;
}

struct rcvseg RcvBuffer::getElement_i(int i){
    return winBuff[i];
}
