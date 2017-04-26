/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <sys/epoll.h>

#include <vector>
#include <map>

#include "NonCopyable.h"
#include "DefineVal.h"
#include "SOCKAcceptor.h"
#include "DefineVal.h"

class Util;
class SOCKAcceptor;
class Epoll : public NonCopyable{
    public:
        Epoll();
        ~Epoll();

        void getSockAcceptorInfo(SOCKAcceptor *sockAcceptor);
        void monitor();

        void addEvent(const int &fd, const int &state);
        void deleteEvent(const int &fd, const int &state);
        void modifyEvent(const int &fd, const int &state);

        void handleEvents(int eventNum, int listenfd);
        void handleAccept(int listenfd);

        int getEpollfd() const;

        void addFifoFdFromClient(int fifoFd);
        void addFifoFdToClient(int fifoFdFromClient,
                int fifoFdToClient);
    private:
        int _epollfd;
        struct epoll_event events[EPOLLEVENTS];
        char buf[RECVMAXSIZE];
        int _sockfd;

        std::vector<int> _fifoFdFromClientVec;
        //first is fdFromClient, second is fdToClient
        std::map<int, int> _fifoMap;

        SOCKAcceptor*_sockAcceptor;

};

#endif /*_EPOLL_H_*/
