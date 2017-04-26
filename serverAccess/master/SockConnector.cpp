/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "SockConnector.h"
#include "DefineVal.h"
#include "MutexLock.h"
#include "Util.h"

int SockConnector::_sockfd = socket(AF_INET,
        SOCK_STREAM, 0);

MutexLock SockConnector::_mutex;

SockConnector *SockConnector::_sockConnector = new SockConnector();

SockConnector *SockConnector::getInstance(){
    if(_sockConnector == NULL){
        SockConnector::_mutex.lock();
        if(_sockConnector == NULL){
            _sockConnector = new SockConnector();
        }
        SockConnector::_mutex.unlock();
    }
    return _sockConnector;
}


void SockConnector::sockConnect(const char *ip, const short &port){
    SockConnector::_mutex.lock();
    if(SockConnector::_sockfd < 0){
#ifdef DEBUG
        std::cout << "socket error" << std::endl;
#endif /*DEBUG*/
        exit(-1);
    }

    int optval;
    setsockopt(SockConnector::_sockfd, SOL_SOCKET,
            SO_LINGER, &optval, sizeof(optval));

    struct sockaddr_in server2Addr;
    bzero(&server2Addr, sizeof(server2Addr));
    server2Addr.sin_family = AF_INET;
    server2Addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server2Addr.sin_addr);

    int con = connect(_sockfd,
            (struct sockaddr *)&server2Addr,
            sizeof(server2Addr));
    if(con < 0){
#ifdef DEBUG
        std::cout << "connect error" << std::endl;
#endif
        exit(-1);
    }
#ifdef NONBLOCK
    CUtil::setNonblock(_sockfd);
#endif /*NONBLOCK*/
    SockConnector::_mutex.unlock();
    std::cout << "connect done" << std::endl;
}
