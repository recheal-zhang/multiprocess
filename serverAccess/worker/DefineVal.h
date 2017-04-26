/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#ifndef _DEFINEVAL_H_
#define _DEFINEVAL_H_
#include <string>
#include <sys/epoll.h>

#define INVALID_SOCKFD_VALUE 0

#define IPADDRESS "127.0.0.1"
#define PORT 124
#define RECVMAXSIZE 1024
#define LISTENQ 5
#define FDSIZE 1000
#define EPOLLEVENTS 100
#define MAXTHREADNUM 1
#define TIMEOUT 500

#define SVRADDRESS "127.0.0.1"
#define SVRPORT 126

#define LOGTHREADSLEEPTIME 20

struct clientMsg{
    int clientAcceptFd;
    char msg[RECVMAXSIZE];
    int length;
    //TODO: should extend
};

struct serverProcessMsg{
    int serverConnectFd;
    bool serverMd5Result;
};

struct SBufferNode{
    int epollfd;
//    char *buf;
    struct clientMsg cliMsg;
    struct serverProcessMsg svrProMsg;

    struct epoll_event event;
};


#endif /*_DEFINEVAL_H_*/


