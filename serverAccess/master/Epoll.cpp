/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#include <iostream>

#include <string.h>
#include <unistd.h>

#include "Epoll.h"
#include "Util.h"
#include "INETAddr.h"
#include "SockConnector.h"

#include "DefineVal.h"

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


void Epoll::getSockAcceptorInfo(SOCKAcceptor *sockAcceptor){
    _sockAcceptor = sockAcceptor;
    addEvent(_sockAcceptor->_sockfd, EPOLLIN | EPOLLET);
    _sockfd = _sockAcceptor->_sockfd;

    memset(buf, 0, RECVMAXSIZE);
}

void Epoll::addFifoFdFromClient(int fifoFd){
    addEvent(fifoFd, EPOLLIN | EPOLLET);
    _fifoFdFromClientVec.push_back(fifoFd);
}

void Epoll::addFifoFdToClient(int fifoFdFromClient,
        int fifoFdToClient){
    _fifoMap[fifoFdFromClient] = fifoFdToClient;
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
            int nread;

            //----------------------------------------
            //if the msg come from server2
            //TODO: Md5
            if(fd == SockConnector::_sockfd){
                //TODO:read, write into shareMemory
                //and send msg to worker via fifo
            }
            else if(fd == listenfd){
                handleAccept(listenfd);
            }
            else if(CUtil::findFifoFdFromVec(
                        _fifoFdFromClientVec,
                        fd
                        )){
                //come from client FIFO
                bzero(buf, RECVMAXSIZE);
                CUtil::readMsgFromFifo(fd, buf, RECVMAXSIZE);
                std::cout << buf << std::endl;
                bzero(buf, RECVMAXSIZE);

                char msg[5] = "0000";
                std::cout << "_fifoMap[" << fd << "]="
                    <<_fifoMap[fd] << std::endl;
                CUtil::writeMsgToFifo(_fifoMap[fd],
                        msg,
                        5);

            }
            else{
                //TODO: if(fd == fifo's fd) ?
            }
        }

        else if(events[i].events & EPOLLOUT){
            //TODO:
        }

        else if((events[i].events & EPOLLERR) ){
#ifdef DEBUG
            std::cout << "epoll error" << std::endl;
#endif /*DEBUG*/
            close(fd);
        }
    }
}

void Epoll::handleAccept(int listenfd){
    int clifd = _sockAcceptor->cliAccept();
    if(clifd == -1){
#ifdef DEBUG
        std::cout << "accept error" << std::endl;
#endif /*DEBUG*/
        return ;
    }

#ifdef NONBLOCK
    CUtil::setNonblock(clifd);
#endif /*NONBLOCK*/

    addEvent(clifd, EPOLLIN | EPOLLET);
}

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



