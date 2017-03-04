//
//  OutputBuffer.cpp
//  RDT Project2
//
//  Created by Hao WU on 3/3/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#include "OutputBuffer.h"
OutputBuffer::OutputBuffer(int Date_num){
    size=Date_num;
    my_outputbuffer=std::vector<Data>(Date_num,{'C'});// Test it and loaded with mutiple 'C'
}

OutputBuffer::OutputBuffer(){
    
}

bool OutputBuffer::isEmpty(){
    return size==0;
}

Data OutputBuffer::retreive_last(){
    Data last=my_outputbuffer.back();
    my_outputbuffer.pop_back();
    size--;
    return last;
}
