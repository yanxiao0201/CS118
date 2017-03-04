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
#include "TCPServer.h"

int main(int argc, const char * argv[]) {
    if (argc!=3) {
        std::cout<<"Usage: server + Port_number + Filename\n";
        exit(1);
    }
    int port=atoi(argv[1]);
    TCPServer myserver(port);
    myserver.receiveSYN();
    myserver.sendSYNACK();
    myserver.receivefirstACK();
    myserver.sendFile();
    myserver.sendFIN();
    myserver.receiveFINACK();
    return 0;
}
