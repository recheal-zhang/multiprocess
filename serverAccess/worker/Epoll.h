/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <sys/epoll.h>


#include "NonCopyable.h"
#include "DefineVal.h"
#include "DefineVal.h"
#include "ShareMemory.h"

class Util;
class CShareMemory;
class Epoll : public NonCopyable{
    public:
        Epoll();
        ~Epoll();

        void addShmToMasterInfo(CShareMemory *);
        void addShmFromMasterInfo(CShareMemory *);

        void addFifoWriteFdInfo(int);
        void monitor();

        void addEvent(const int &fd, const int &state);
        void deleteEvent(const int &fd, const int &state);
        void modifyEvent(const int &fd, const int &state);

        void handleEvents(int eventNum, int listenfd);
 //       void handleAccept(int listenfd);

        int getEpollfd() const;

        void addFifoFdFromServer(int fifoFd);
    private:
        int _epollfd;
        struct epoll_event events[EPOLLEVENTS];
        char buf[RECVMAXSIZE];
        int _sockfd;

        int _fifoFdFromServer;
        int _fifoFdToServer;

        CShareMemory *_shmToMaster;
        CShareMemory *_shmFromMaster;


};

#endif /*_EPOLL_H_*/
