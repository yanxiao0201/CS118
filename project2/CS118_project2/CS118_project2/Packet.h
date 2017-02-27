//
//  packet.hpp
//  CS118_project2
//
//  Created by Xiao Yan on 2/27/17.
//  Copyright © 2017 Xiao Yan. All rights reserved.
//

#ifndef Packet_h
#define Packet_h

#include <stdio.h>
#include <vector>
#include <stdlib.h>
typedef std::vector<uint8_t> Data;

struct TCPheader{
    uint16_t seq_number = 0;
    uint16_t ack_number = 0;
    //uint16_t rcvWin = 0;
    bool ACK=false;
    bool FIN=false;
    bool SYN=false;
};

class Packet{
private:
    TCPheader myHeader;
    Data myData;
public:
    Packet();
    void setHeader(uint16_t seq_number,uint16_t ack_number,bool ACK,bool FIN,bool SYN);
    TCPheader getHeader();
    void setData(Data out_data);
    Data getData();
    void setSeq(uint16_t seq_number);
    void setAckNumber(uint16_t ack_number);
    uint16_t getSeq();
    uint16_t getAckNumber();
    void setACK(bool ACK);
    void setFIN(bool FIN);
    void setSYN(bool SYN);
    bool getACK();
    bool getFIN();
    bool getSYN();
};
#endif /* Packet_hpp */
