//
//  RcvBuffer.cpp
//  CS118_project2
//
//  Created by Xiao Yan on 3/5/17.
//  Copyright © 2017 Xiao Yan. All rights reserved.
//

#include "RcvBuffer.h"

//#include "Packet.h"

RcvBuffer::RcvBuffer(){
    
    for (int i = 0 ; i < WNDSIZE; i++){
        rcvseg tmp;
        tmp.seqNo = 0;
        tmp.isACKed = false;
        winBuff.push_back(tmp);
    }

    rcv_base = 0;
    base_status = false;

}

bool RcvBuffer::insert_and_assert(Packet& p){
    
    Data tmp = p.getData();
    uint16_t seqNo = p.getSeq();
    
    if (tmp.size() == 0){
        return false;
    }
    
    int pos;

    for (int i = 0; i < WNDSIZE; i++){
        pos = (rcv_base + DATASIZE * i) % MAXSEQ;
        if (seqNo == pos){
            //if a reasonable package
            if (isAcked_i(i)){
                return true;
            }
            else{
                setAcked_i(i);
                setData_i(i, tmp);
                setseqNo_i(i, seqNo);
                return true;
            }
        }
    }
    
    int out;
    for (int i = 1 ; i < WNDSIZE + 1; i++){
        out = (rcv_base - DATASIZE * i) % MAXSEQ;
        if (out < 0){
            return false;
        }
        if (seqNo == out){
            //this is a previously acked package
            return true;
        }
    }
    return false;
}

void RcvBuffer::write(std::fstream& outfile){
    while (isAcked_i(0)){
        //the first one is acked
        rcvseg tmp = pop_front();
        outfile.write(tmp.thisData.data(),tmp.thisData.size());
        push_back();
        
    }
}


bool RcvBuffer::isAcked_i(int i){
    return winBuff[i].isACKed;
    
}

void RcvBuffer::setAcked_i(int i){
    winBuff[i].isACKed = true;
}

rcvseg RcvBuffer::pop_front(){
    rcvseg res = winBuff[0];
    winBuff.pop_front();
    rcv_base = (rcv_base + DATASIZE) % MAXSEQ;
    return res;
}

void RcvBuffer::push_back(){
    rcvseg tmp;
    tmp.isACKed = false;
    tmp.seqNo = 0;
    winBuff.push_back(tmp);
}

void RcvBuffer::setData_i(int i,Data& data){
    winBuff[i].thisData = data;
}

void RcvBuffer::setseqNo_i(int i, uint16_t seqNo){
    winBuff[i].seqNo = seqNo;
}

std::deque<rcvseg> RcvBuffer::getBuffer(){
    return winBuff;
}

struct rcvseg RcvBuffer::getElement_i(int i){
    return winBuff[i];
}

uint16_t RcvBuffer::get_rcv_base(){
    return rcv_base;
}

void RcvBuffer::set_rcv_base(uint16_t base){
    if (base_status == false){
        rcv_base = base;
        base_status = true;
    }
}

int RcvBuffer::size(){
    return winBuff.size();
}

bool RcvBuffer::is_base_set(){
    return base_status;
}









