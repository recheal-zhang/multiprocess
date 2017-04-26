/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#ifndef _DEFINEVAL_H_
#define _DEFINEVAL_H_

#include <string>
#include <sys/epoll.h>

#define INVALID_SOCKFD_VALUE 0

#define TIMEOUT 500
#define RECVMAXSIZE 1024



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

struct threadMsg{
    int epollfd;
//    char *buf;
    struct clientMsg cliMsg;
    struct serverProcessMsg svrProMsg;

    struct epoll_event event;
};


#endif /*_DEFINEVAL_H_*/


