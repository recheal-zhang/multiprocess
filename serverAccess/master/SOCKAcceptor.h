/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#ifndef _SOCKACCEPTOR_H_
#define _SOCKACCEPTOR_H_

#include <unistd.h> //close

#include "DefineVal.h"


class INETAddr;
class Epoll;

class SOCKAcceptor{
        friend class Epoll;
    public:
        SOCKAcceptor();
        ~SOCKAcceptor(){close(_sockfd);}

        void sockBindListen();
        void bindListen(const INETAddr &sockAddr);
        //Accept a connection and initialize the <stream>
        int cliAccept();


        int getSockfd() const;
        void setSockfd(int sockfd);

        //I/O operations
        int sockRecv(void *buffer, size_t len , int flags);
        int sockSend(const void *buffer, size_t len, int flags);


    private:
        int _sockfd;
};

#endif /*_SOCKACCEPTOR_H_*/
