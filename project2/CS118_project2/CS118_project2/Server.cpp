//
//  main.cpp
//  RDT Project2
//
//  Created by Hao WU on 2/27/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "Packet.h"
int startUDP(int port){// it will start, and bind to a port, then start listening, then return the fd of
    struct sockaddr_in my_address;
    int sockid=socket(AF_INET, SOCK_DGRAM, 0);// return the file descriptor of the new socket
    if (sockid==-1) {
        perror("Socket Creation Failed");
        exit(1);
    }
    memset((char*)&my_address, 0, sizeof(my_address));
    my_address.sin_family=AF_INET;
    my_address.sin_port=htons(port);
    my_address.sin_addr.s_addr=htonl(INADDR_ANY);
    if (bind(sockid, (struct sockaddr*) &my_address, sizeof(my_address))==-1) {// bind socket to add port
        perror("Bind Failed");
        exit(1);
    }
    return sockid;
}

int receiveSYN(int socket_fd){
    ssize_t receive_len=-1;
    char buffer[BUFFSIZE];
    struct sockaddr_in client_address;
    socklen_t clenet_addrlen=sizeof(client_address);
    while (true) {
        memset(buffer, 0, BUFFSIZE);
        receive_len=recvfrom(socket_fd, buffer, BUFFSIZE, 0, (struct sockaddr*) &client_address, &clenet_addrlen);
        if (receive_len<0) {
            perror("Receive SYN error\n");
            exit(1);
        }
        Data mydata(buffer,buffer+receive_len);
        Packet recv_pkt(mydata);
        if (recv_pkt.getSYN()) {
            printf("SYN received!");
            return recv_pkt.getSeq()+1;
        }
        else{
            perror("SYN packet garbled\n");
            continue;
        }
    }
}

int main(int argc, const char * argv[]) {
    if (argc!=3) {
        std::cout<<"Usage: server + Port_number + Filename\n";
        exit(1);
    }
    int port=atoi(argv[1]);
    int socket_fd=startUDP(port);
    receiveSYN(socket_fd);
    return 0;
}
