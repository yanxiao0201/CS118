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

struct rcvseg{
    uint16_t seqNo;
    Data thisData;
};

class RcvBuffer{
public:
    int insert(Packet p);
    int buffsize();
    void clean();
    std::vector<rcvseg> getBuffer();

private:
    std::vector<rcvseg> FinalBuff;
};


#endif /* client_h */
