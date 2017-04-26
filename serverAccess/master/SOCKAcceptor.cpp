/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#include <iostream>
#include <string>

#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

#include "SOCKAcceptor.h"
#include "DefineVal.h"
#include "INETAddr.h"
#include "Util.h"

SOCKAcceptor::SOCKAcceptor(){
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);

#ifdef DEBUG
    if(_sockfd == -1){
        std::cout << "socket error in SOCKAcceptor" << std::endl;
    }
#endif /*DEBUG*/

#ifdef NONBLOCK
    int flags = fcntl(_sockfd, F_GETFL, 0);
    fcntl(_sockfd, F_SETFL, flags | O_NONBLOCK);
#endif /*NONBLOCK*/

}

void SOCKAcceptor::sockBindListen(){
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr);
    servaddr.sin_port = htons(PORT);
    if(bind(_sockfd, (struct sockaddr*)&servaddr,
                sizeof(servaddr)) == -1){
#ifdef DEBUG
        std::cout << "bind error" << std::endl;
        exit(-1);
#endif /*DEBUG*/
        return;
    }

    listen(_sockfd, LISTENQ);

}

void SOCKAcceptor::bindListen(const INETAddr &sockAddr){
    //Associate address with endpoint.
    std::cout << "addr:" << sockAddr.getAddr() << std::endl;
    std::cout << "port" << sockAddr.getPort() << std::endl;
    std::cout << "size:" << sockAddr.getSize() << std::endl;
    bind(_sockfd,
            sockAddr.getAddr(),
            sockAddr.getSize());

    //Make endpoint listen for connections
    listen(_sockfd, LISTENQ);

    std::cout << "listenfd in SOCKACCEPTOR= " << _sockfd << std::endl;

}

int SOCKAcceptor::cliAccept(){
    //TODO: use pair for accept return ?
    //accept(_sockfd, struct sockaddr*, socklen_t*)
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;
    std::cout << "accept in" << std::endl;
     int clifd = accept(_sockfd, (struct sockaddr *)&cliaddr, &cliaddrlen);
#ifdef NONBLOCK
    CUtil::setNonblock(clifd);
#endif /*NONBLOCK*/
    return clifd;
}

int SOCKAcceptor::getSockfd() const{
    return _sockfd;
}

void SOCKAcceptor::setSockfd(int sockfd){
    _sockfd = sockfd;
}

int SOCKAcceptor::sockRecv(void *buffer, size_t len = RECVMAXSIZE, int flags = 0){
    int ret = recv(_sockfd, buffer, len, flags);
    return ret;
}

int SOCKAcceptor::sockSend(const void *buffer, size_t len, int flags = 0){
    int ret = send(_sockfd, buffer, len, flags);
    return ret;
}


