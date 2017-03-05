//
//  OutputBuffer.cpp
//  RDT Project2
//
//  Created by Hao WU on 3/3/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#include "OutputBuffer.h"
#include <iostream>
#include <fstream>
OutputBuffer::OutputBuffer(int Data_num){
    my_outputbuffer_queue=std::deque<Data>(Data_num,{'C'});
}

OutputBuffer::OutputBuffer(){
    
}

bool OutputBuffer::IsEmpty(){
    return my_outputbuffer_queue.empty();
}

Data OutputBuffer::retrieve_last(){
    Data last=my_outputbuffer_queue.back();
    my_outputbuffer_queue.pop_back();
    //std::cout<<"size is "<<size<<std::endl;
    return last;
}

Data OutputBuffer::retrieve_front(){
    Data first=my_outputbuffer_queue.front();
    my_outputbuffer_queue.pop_front();
    return first;
}

int OutputBuffer::getSize(){
    return my_outputbuffer_queue.size();
}

void OutputBuffer::readFile(std::string filename){
    std::streampos size;
    char *memblock;
    std::ifstream file(filename,std::ios::in|std::ios::ate|std::ios::binary);
    if(file.is_open()){
        size=file.tellg();
        memblock=new char[size];
        file.seekg(0,std::ios::beg);
        file.read(memblock,size);
        file.close();
        int i=0;
        while(i+BUFFSIZE-7<size){
            Data data_peice(memblock+i,memblock+i+BUFFSIZE-7);
            my_outputbuffer_queue.push_back(data_peice);
            i+=BUFFSIZE-7;
        }
        Data data_peice(memblock+i,memblock+size);
        my_outputbuffer_queue.push_back(data_peice);
    }
    else{
        std::cout << "Unable to open file";
    }
    return ;
}

