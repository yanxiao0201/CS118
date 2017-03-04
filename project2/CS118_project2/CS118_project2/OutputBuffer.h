//
//  OutputBuffer.hpp
//  RDT Project2
//
//  Created by Hao WU on 3/3/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#ifndef OutputBuffer_h
#define OutputBuffer_h

#include "Packet.h"
class OutputBuffer{
public:
    OutputBuffer();
    OutputBuffer(int Data_num);
    Data retreive_last();
    bool isEmpty();
private:
    std::vector<Data> my_outputbuffer;
    int size;
};
#endif /* OutputBuffer_hpp */
