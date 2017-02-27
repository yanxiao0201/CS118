
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

int start(int port){// it will start, and bind to a port, then start listening, then return the fd of
    struct sockaddr_in addport;
    int sockid=socket(AF_INET, SOCK_STREAM, 0);// return the file descriptor of the new socket
    if (sockid==-1) {
        perror("Socket Creation Failed");
        exit(1);
    }
    memset((char*)&addport, 0, sizeof(addport));
    addport.sin_family=AF_INET;
    addport.sin_port=htons(port);
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

int read_line(int socketID,char *buffer,int size){// This function will convert all the new line signs to the '\n'
    int count=0;
    int c='\0';
    while ((count<size-1) && (c!='\n')) {
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
/*
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
 */

int get_method(int socketID, int location, char* buff){
    if (strncmp(buff, "GET", 3) == 0){
        //if the method is GET
        //DOSTUFF
        location = location + 3;
    }
    else{
        location = -2;
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
    return i;
}

/*
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
 */
void GetRequestHandler (int socketID,char *path,char *ContentType){
    char *source = NULL;
    FILE *fp = fopen(path,"r");
    
    //if no file exists, generate a 404 message
    if (fp == NULL){
        write(socketID,"HTTP/1.1 404 Not Found\r\n",strlen("HTTP/1.1 404 Not Found\r\n"));
        write(socketID, "Server: webserver\r\n",strlen("Server: webserver\r\n"));
        write(socketID, "Content-Length: 64\r\n", strlen("Content-Length: 64\r\n"));
        write(socketID, "Content-Type: ",strlen("Content-Type: "));
        write(socketID, "text/html", strlen("text/html"));
        write(socketID, "\r\n", strlen("\r\n"));
        write(socketID, "Connection: close\r\n\r\n", strlen("Connection: close\r\n\r\n"));
        write(socketID, "<html>\r\n", strlen("<html>\r\n"));
        write(socketID, "<h1>404 Not Found</h1>\r\n", strlen("<h1>404 Not Found</h1>\r\n"));
        write(socketID, "</html>\r\n", strlen("</html>\r\n"));
        close(socketID);
        
        printf("HTTP/1.1 404 Not Found\r\n");
        printf("Server: webserver\r\n");
        printf("Content-Length: 64\r\n");
        printf("Content-Type: text/html\r\n");
        printf("Connection: close\r\n\r\n");
        return;
    }
    else{
        char conlength[256];
        memset(conlength,0,256);

        fseek(fp, 0L, SEEK_END);
        int fsize = ftell(fp);
        source = malloc(sizeof(char) * (fsize + 1));
        fseek(fp, 0L, SEEK_SET);
        int sourceLength = fread(source, sizeof(char), fsize, fp);
        source[sourceLength] = '\0';
        sprintf(conlength,"Content-Length: %d\r\n",sourceLength);
        
        write(socketID,"HTTP/1.1 200 OK\r\n",strlen("HTTP/1.1 200 OK\r\n"));
        write(socketID, "Server: webserver\r\n",strlen("Server: webserver\r\n"));
        write(socketID, conlength, strlen(conlength));
        write(socketID, "Content-Type: ",strlen("Content-Type: "));
        write(socketID, ContentType, strlen(ContentType));
        write(socketID, "\r\n", strlen("\r\n"));
        write(socketID, "Connection: keep-alive\r\n", strlen("Connection: keep-alive\r\n"));
        write(socketID,"\r\n",2);
        write(socketID,source,sourceLength);
        free(source);
        
        printf("%s","sending file...\nResponse Header: \n\n");
        printf("HTTP/1.1 200 OK\r\n");
        printf("Server: webserver\r\n");
        printf("%s",conlength);
        printf("Content-Type: ");
        printf("%s", ContentType);
        printf("\r\n");
        printf("Connection: keep-alive\r\n");
        printf("\r\n");
        return;
    }
}

void BadRequestHandler(int socketID){
    write(socketID,"HTTP/1.1 400 Bad Request\r\n",strlen("HTTP/1.1 400 Bad Request\r\n"));
    write(socketID, "Server: webserver\r\n",strlen("Server: webserver\r\n"));
    write(socketID, "Content-Length: 64\r\n", strlen("Content-Length: 64\r\n"));
    write(socketID, "Content-Type: ",strlen("Content-Type: "));
    write(socketID, "text/html", strlen("text/html"));
    write(socketID, "\r\n", strlen("\r\n"));
    write(socketID, "Connection: close\r\n\r\n", strlen("Connection: close\r\n\r\n"));
    write(socketID,"\r\n",2);
    write(socketID, "<html>\r\n", strlen("<html>\r\n"));
    write(socketID, "<h1>400 Bad Request</h1>\r\n", strlen("<h1>400 Bad Request</h1>\r\n"));
    write(socketID, "</html>\r\n", strlen("</html>\r\n"));
    close(socketID);
    
    printf("HTTP/1.1 400 Bad Request\r\n");
    printf("Server: webserver\r\n");
    printf("Content-Length: 64\r\n");
    printf("Content-Type: text/html\r\n");
    printf("Connection: keep-alive\r\n\r\n");
    return;
}

void readFileType(char *in_buffer,int startpos,char *out_buffer,int out_buffer_size){
    const char *supported_type[3];
    supported_type[0]="html";
    supported_type[1]="jpg";
    supported_type[2]="gif";
    const char *supported_cat[3];
    supported_cat[0]="text/";
    supported_cat[1]="image/";
    supported_cat[2]="image/";
    int i=startpos-1;
    int j=0;
    char temp[256];
    while(i>=0 && in_buffer[i]!=' ' && in_buffer[i]!='.'){
        i--;
    }
    if (in_buffer[i]=='.') {
        i++;
        while( j<255 && isalpha(in_buffer[i]) ){
            temp[j]=in_buffer[i];
            i++;
            j++;
        }
        int t=0;
        for (t=0; t<3; t++) {
            if (strcmp(temp, supported_type[t])==0) {
                strcpy(out_buffer,supported_cat[t]);
                strcat(out_buffer, supported_type[t]);
                return;
            }
        }
        strcpy(out_buffer, supported_cat[0]);
        strcat(out_buffer, supported_type[0]);
        return;
    }
    else{//if this is an unsopported data type return html
        strcpy(out_buffer, supported_cat[0]);
        strcat(out_buffer, supported_type[0]);
        return;
    }
}

void HTTPRequestHandler(int *socketID){
    int socketID_copy=*socketID;
    int location=0;
    char line1_buffer[1024];
    char ContentType[256];
    char URL[256];
    char path[512];
    char rest_buffer[1024];
    //read the first line of HTTP request to line1_buffer and the rest of HTTP buffer to rest_buffer
    read_line(socketID_copy, line1_buffer, sizeof(line1_buffer));
    read(*socketID, rest_buffer, 1024);
    //dump the request to console
    printf("%s",line1_buffer);
    printf("%s",rest_buffer);
    printf("\r\n");
    
    //process the line1_buffer
    location=get_method(socketID_copy, location, line1_buffer);
    if (location<0) {
        BadRequestHandler(socketID_copy);
        return;
    }
    location=readURL(line1_buffer, location, sizeof(line1_buffer), URL, sizeof(URL));
    readFileType(line1_buffer, location,ContentType,sizeof(ContentType));
    sprintf(path, "www%s",URL);
    if (path[strlen(path)-1]=='/') {// if the enter path is a directory, return the index.html file
        strcat(path, "index.html");
    }
    GetRequestHandler(socketID_copy,path,ContentType);
}


int main(int argc, const char * argv[]) {
    if (argc!=2) {
        printf("%s\n","Segmentation Fault: try to use \" server + Port Number (>1024 !=6789 !=8080) \"");
        exit(1);
    }
    u_short port= atoi(argv[1]);
    int server_socketID=start(port);
    struct sockaddr_in myport;
    unsigned int myport_len=sizeof(myport);
    int refer_server_socketID=-1;
    while (1) {
        refer_server_socketID=accept(server_socketID, (struct sockaddr*)&myport, &myport_len);
        if (refer_server_socketID==-1) {
            perror("accept error");
            exit(1);
        }
        printf("HTTP request accepted: \n\n");
        if (!fork()) {
            close(server_socketID);
            HTTPRequestHandler(&refer_server_socketID);
            close(refer_server_socketID);
            exit(0);
        }
        close(refer_server_socketID);// must close it
    }
    close(server_socketID);
    close(refer_server_socketID);
    return 0;
}
