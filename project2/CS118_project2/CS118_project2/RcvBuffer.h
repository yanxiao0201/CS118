//
//  RcvBuffer.hpp
//  CS118_project2
//
//  Created by Xiao Yan on 3/5/17.
//  Copyright © 2017 Xiao Yan. All rights reserved.
//

#ifndef RcvBuffer_hpp
#define RcvBuffer_hpp

#include "Packet.h"
#include <deque>
#include <fstream>

struct rcvseg{
    uint16_t seqNo;
    bool isACKed;
    Data thisData;
};

class RcvBuffer{
public:
    RcvBuffer();
    bool insert_and_assert(Packet& p);
    void write(std::fstream& outfile);
    
    rcvseg pop_front();
    void push_back();
    
    bool isAcked_i(int i);
    void setAcked_i(int i);
    
    void setData_i(int i, Data& data);
    void setseqNo_i(int i, uint16_t seqNo);
    
    std::deque<rcvseg> getBuffer();
    struct rcvseg getElement_i(int i);
    
    uint16_t get_rcv_base();
    void set_rcv_base(uint16_t base);
    bool is_base_set();
    
    int size();
    
    
private:
    std::deque<rcvseg> winBuff;
    uint16_t rcv_base;
    bool base_status;
};



#endif /* RcvBuffer_hpp */
