//
//  client.h
//  CS118_project2
//
//  Created by Xiao Yan on 3/1/17.
//  Copyright © 2017 Xiao Yan. All rights reserved.
//

#ifndef client_h
#define client_h

Data packet_generator(Packet& rcv_packet);
//Data request_generator(Packet& rcv_packet, char * filename);
Data syn_generator(void);

class RcvBuffer{
public:
    RcvBuffer();
    void setSeq(uint16_t seq_number);
    uint16_t getSeqNo();
    int insert(Packet p);
    std::vector<char> getBuffer();

private:
    std::vector<char> buffer;
};






#endif /* client_h */
