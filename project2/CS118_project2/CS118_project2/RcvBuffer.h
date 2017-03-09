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

struct rcvseg{
    uint16_t seqNo;
    bool ACKed;
    Data thisData;
};

class RcvBuffer{
public:
    int insert(Packet p);
    int buffsize();
    void clean();
    std::vector<rcvseg> getBuffer();
    struct rcvseg getElement_i(int i);
    bool isACKed_i(int i);
    
private:
    std::vector<rcvseg> winBuff;
};



#endif /* RcvBuffer_hpp */
