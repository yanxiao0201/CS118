//
//  main.c
//  HTTP_Server
//
//  Created by Hao WU on 1/16/17./Users/xiaoyan/Library/Containers/com.apple.mail/Data/Library/Mail Downloads/1DD78E4C-AC41-4554-A569-79AB7A781E42/main.c
//  Copyright © 2017 Hao WU. All rights reserved.
//

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

int start(){// it will start, and bind to 5100 port, then start listening, then return the fd of
    struct sockaddr_in addport;
    int sockid=socket(AF_INET, SOCK_STREAM, 0);// return the file descriptor of the new socket
    if (sockid==-1) {
        perror("Socket Creation Failed");
        exit(1);
    }
    memset((char*)&addport, 0, sizeof(addport));
    addport.sin_family=AF_INET;
    addport.sin_port=htons(5100);// 5100 port
    addport.sin_addr.s_addr=htonl(INADDR_ANY);
    if (bind(sockid, (struct sockaddr*) &addport, sizeof(addport))==-1) {// bind socket to add port
        perror("Bind Failed");
        exit(1);
    }
    if (listen(sockid, 5)<0) {// 10 is the maximum length of pending connection
        perror("Listen Failed");
        exit(1);
    }
    return sockid;
}
/*
void consoledump(int socketID){
    char buffer[1024];
    bzero(buffer, 1024);
    if(read(socketID, buffer, 1023)<0){
        perror("Reading Error");
        exit(1);
    }
    printf("%s",buffer);
    write(socketID, "I got your message", 18);
    return;
}
 */

int read_line(int socketID,char *buffer,int size){// This function will convert all the new line signs to the '\n'
    int count=0;
    int c='\0';
    while ( (count<size-1) && (c!='\n')) {
        ssize_t n=recv(socketID, &c, 1, 0);
        if (n<0) {
            return -1;// read error, break the loop
        }
        else{
            if (c=='\r') {
                n=recv(socketID, &c, 1, MSG_PEEK);
                if ((n>0) && (c=='\n')) {
                    n=recv(socketID, &c, 1, 0);
                }
                else{// MacOS has a single '\r' as new line
                    c='\n';
                }
            }
            buffer[count]=c;
            count++;
        }
    }
    buffer[count]='\0';
    return count;
}

/*Function to debug read_lines*/
void test_read_line(int socketID){
    char buffer[1024];
    bzero(buffer, 1024);
    if(read_line(socketID, buffer, 1023)<0){
        perror("Reading Error");
        exit(1);
    }
    printf("%s",buffer);
    write(socketID, "I got your message", 18);
    return;
}

int get_method(int socketID, int location, char* buff){
    if (strncmp(buff, "GET", 3) == 0){
        //if the method is GET
        //DOSTUFF
        location = location + 3;
    }
    else{
        write(socketID,"400 Bad Request\r\n",26);
        printf("%s","HTTP/1.1 400 Bad Request\n");
        
        exit(-1);
    }
    return location+1;
}

int readURL(char *in_buffer,int startpos,int in_buffer_size, char *out_buffer,int out_buffer_size){
    int i=startpos;
    int j=0;//j is the index of out_buffer
    while (in_buffer[i]==' ' && i<in_buffer_size) {
        i++;
    }
    while (in_buffer[i]!=' ' && i<in_buffer_size && j<out_buffer_size-1) {
        out_buffer[j]=in_buffer[i];
        j++;
        i++;
    }
    out_buffer[j]='\0';
    return j;
}

void test_readURL(int socketID){//only works for GET
    char buffer[1024];
    char URL[512];
    bzero(buffer, 1024);
    if(read_line(socketID, buffer, 1023)<0){
        perror("Reading Error");
        exit(1);
    }
    printf("%s",buffer);
    printf("%s","TestingURL...\n");
    readURL(buffer, 4, sizeof(buffer), URL, sizeof(URL));
    printf("%s",URL);
    write(socketID, "I got your message", 18);
    return;
}

void send_file (int socketID,char *path){
    printf("%s","sending file...\n");
    char *source = NULL;
    FILE *fp = fopen(path,"r");
    
    //if no file exists, generate a 404 message
    if (fp == NULL){
        write(socketID, "404 Not Found\r\n",15);
        printf("%s","HTTP/1.1 404 Not Found\r\n");
        exit(-1);
    }
    
    write(socketID,"HTTP/1.1 200 OK\r\n",18);
    write(socketID, "Connection: keep-alive\r\n", 26-2);
    write(socketID,"\r\n",2);
    printf("%s","HTTP/1.1 200 OK\r\n");
    
    fseek(fp, 0L, SEEK_END);
    int fsize = ftell(fp);
    source = malloc(sizeof(char) * (fsize + 1));
    fseek(fp, 0L, SEEK_SET);
    int sourceLength = fread(source, sizeof(char), fsize, fp);
    source[sourceLength] = '\0';
    write(socketID,source,sourceLength);
    free(source);
    //printf("%s",path);
    //printf("%d",sourceLength);
}

void parsing_request(int *socketID){
    int socketID_copy=*socketID;
    char line1_buffer[1024];
    
    char URL[256];
    char path[512];
    int line1_size=read_line(socketID_copy, line1_buffer, sizeof(line1_buffer));
    int location=0;
    location=get_method(socketID_copy, location, line1_buffer);
    readURL(line1_buffer, location, sizeof(line1_buffer), URL, sizeof(URL));
    
    printf("%s",line1_buffer);
    //memset(line1_buffer, 0, 1024);
    //read(*socketID, line1_buffer, 1024);
    //printf("%s",line1_buffer);
    sprintf(path, "www%s",URL);
    //printf("%s",path);
    if (path[strlen(path)-1]=='/') {// if the enter path is a directory, return the index.html file
        strcat(path, "index.html");
    }
    send_file(socketID_copy,path);
}

//old version

/*int main(int argc, const char * argv[]) {
    //int server_socketID=start();
    u_short port=5100;
    int server_socketID=start();
    printf("%s","start_listening\n");
    struct sockaddr_in myport;
    unsigned int myport_len=sizeof(myport);
    int refer_server_socketID=accept(server_socketID, (struct sockaddr*)&myport, &myport_len);
    parsing_request(&refer_server_socketID);
    close(server_socketID);
    close(refer_server_socketID);
    return 0;
}*/

int main(int argc, const char * argv[]) {
    u_short port=5100;
    int server_socketID=start();
    struct sockaddr_in myport;
    unsigned int myport_len=sizeof(myport);
    int refer_server_socketID=-1;
    while (1) {
        refer_server_socketID=accept(server_socketID, (struct sockaddr*)&myport, &myport_len);
        if (refer_server_socketID==-1) {
            perror("accept error");
            exit(1);
        }
        printf("accepted\n");
        if (!fork()) {
            close(server_socketID);
            parsing_request(&refer_server_socketID);
            close(refer_server_socketID);
            exit(0);
        }
        close(refer_server_socketID);// why must close it?
    }
    close(server_socketID);
    close(refer_server_socketID);
    return 0;
}
