/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#include <iostream>

#include <string.h>
#include <unistd.h>

#include "Epoll.h"
#include "Util.h"

#include "DefineVal.h"
#include "ShmQueue.h"

Epoll::Epoll():
    _epollfd(epoll_create(FDSIZE))
{
    bzero(buf, RECVMAXSIZE);
}

Epoll::~Epoll(){
}

int Epoll::getEpollfd() const{
    return _epollfd;
}

void Epoll::addShmToMasterInfo(CShareMemory *shm){
    if(shm == NULL){
#ifdef DEBUG
        std::cout << "shm is NULL" << std::endl;
#endif /*DEBUG*/
        return;
    }
    _shmToMaster = shm;
}

void Epoll::addShmFromMasterInfo(CShareMemory *shm){
    if(shm == NULL){
#ifdef DEBUG
        std::cout << "shm is NULL" << std::endl;
#endif /*DEBUG*/
        return;
    }
    _shmFromMaster = shm;
}

void Epoll::addFifoWriteFdInfo(int fd){
    _fifoFdToServer = fd;
}

void Epoll::addFifoFdFromServer(int fifoFd){
    addEvent(fifoFd, EPOLLIN | EPOLLET);
    _fifoFdFromServer = fifoFd;
}

void Epoll::monitor(){
    int ret = 0;
    while(true){
        ret = epoll_wait(_epollfd, events, EPOLLEVENTS, TIMEOUT);
        handleEvents(ret, _sockfd);
    }
}

void Epoll::handleEvents(int eventNum, int listenfd){
    int fd;
    for(int i = 0; i < eventNum; i++){
        fd = events[i].data.fd;

        if(events[i].events & EPOLLRDHUP){
            close(fd);
            deleteEvent(fd, EPOLLIN);
        }
        else if(events[i].events & EPOLLIN){
            //TODO: add query num
            if(fd == _fifoFdFromServer){
                //come from client FIFO
                bzero(buf, RECVMAXSIZE);
                CUtil::readMsgFromFifo(fd, buf, RECVMAXSIZE);
  //              std::cout << buf << std::endl;
                bzero(buf, RECVMAXSIZE);

                //read from shmFromMaster
                SBufferNode *node =
                    _shmFromMaster->workerRecvData();
 //               std::cout << (node->cliMsg).msg << std::endl;
                //TODO:zhuanfa
                _shmToMaster->workerSendData(node);

                char msg[5] = "1234";
                CUtil::writeMsgToFifo(_fifoFdToServer, msg, 5);

            }
            else{
                //TODO: if(fd == fifo's fd) ?
            }
        }

        else if(events[i].events & EPOLLOUT){
            //TODO:
        }

        else if((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)){
#ifdef DEBUG
            std::cout << "epoll error" << std::endl;
#endif /*DEBUG*/
            close(fd);
        }
    }
}

//void Epoll::handleAccept(int listenfd){
//    int clifd = _sockAcceptor->cliAccept();
//    if(clifd == -1){
//#ifdef DEBUG
//        std::cout << "accept error" << std::endl;
//#endif /*DEBUG*/
//        return ;
//    }
//
//#ifdef NONBLOCK
//    CUtil::setNonblock(clifd);
//#endif /*NONBLOCK*/
//
//    addEvent(clifd, EPOLLIN | EPOLLET);
//}

void Epoll::addEvent(const int &fd, const int &state){
    struct epoll_event ev;
    ev.events = state | EPOLLET;
    ev.data.fd = fd;
    epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void Epoll::deleteEvent(const int &fd, const int &state){
    struct epoll_event ev;
    ev.events = state | EPOLLET;
    ev.data.fd = fd;
    epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, &ev);
}

void Epoll::modifyEvent(const int &fd, const int &state){
    struct epoll_event ev;
    ev.events = state | EPOLLET;
    ev.data.fd = fd;
    epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &ev);
}



