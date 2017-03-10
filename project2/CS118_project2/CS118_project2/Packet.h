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
#include <cstdint>


#define MAXSEQ 30720
#define BUFFSIZE 1024
#define DATASIZE 1017
#define WNDSIZE 5

typedef std::vector<char> Data;

struct TCPheader{
    uint16_t seq_number;
    uint16_t ack_number;
    //uint16_t rcvWin = 0;
    bool ACK;
    bool FIN;
    bool SYN;
    TCPheader(){
        seq_number=0;
        ack_number=0;
        //uint16_t rcvWin = 0;
        ACK=false;
        FIN=false;
        SYN=false;
    }
};

class Packet{
private:
    TCPheader myHeader;
    Data myData;
public:
    Packet();
    Packet(Data& rcvData);
    void setHeader(uint16_t seq_number,uint16_t ack_number,bool ACK,bool FIN,bool SYN);
    TCPheader getHeader();
    void setData(Data& out_data);
    Data getData();
    void setSeq(uint16_t seq_number);
    void setAckNumber(uint16_t ack_number);
    uint16_t getSeq();
    uint16_t getAckNumber();
    void setACK(bool ACK);
    void setFIN(bool FIN);
    void setSYN(bool SYN);
    bool isACK();
    bool isFIN();
    bool isSYN();
    Data to_UDPData();
    void loadData(Data &ready_to_send_data);
    //int loadPacket();
    //char buffer[BUFFSIZE];
};
#endif /* Packet_hpp */
