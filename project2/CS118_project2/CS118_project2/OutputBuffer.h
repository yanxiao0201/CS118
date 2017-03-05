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
#include <deque>
#include <string>
class OutputBuffer{// Output Buffer are used to store the content
public:
    OutputBuffer();
    OutputBuffer(int Data_num);
    void readFile(std::string filename);
    Data retrieve_last();
    bool IsEmpty();
    //queue type
    Data retrieve_front();
    int getSize();
private:
    std::vector<Data> my_outputbuffer;
    std::deque<Data> my_outputbuffer_queue;
    //int my_size;
};
#endif /* OutputBuffer_hpp */
