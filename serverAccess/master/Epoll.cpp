/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#include <iostream>
#include <utility>

#include <string.h>
#include <unistd.h>

#include "Epoll.h"
#include "Util.h"
#include "INETAddr.h"
#include "SockConnector.h"

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

void Epoll::addShmToWorkerInfo(int fifoFdFromClient,
        CShareMemory *shmToWorker){
    _shmToWorkerMap.insert(std::make_pair(fifoFdFromClient,
                shmToWorker));
}

void Epoll::addShmFromWorkerInfo(int fifoFdFromClient,
        CShareMemory *shmFromWorker){
    _shmFromWorkerMap.insert(std::make_pair(fifoFdFromClient,
                shmFromWorker));
}

void Epoll::getSockAcceptorInfo(SOCKAcceptor *sockAcceptor){
    _sockAcceptor = sockAcceptor;
    addEvent(_sockAcceptor->_sockfd, EPOLLIN | EPOLLET);
    _sockfd = _sockAcceptor->_sockfd;

    memset(buf, 0, RECVMAXSIZE);
}

void Epoll::getSockConnectorInfo(SockConnector *sockConnector){
    addEvent(sockConnector->_sockfd, EPOLLIN);
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
            if(fd == SockConnector::_sockfd){
                int nread;
                //send it to thread which send msg to client
                if((nread = read(fd, &_Msg, sizeof(SBufferNode))) < 0){
#ifdef DEBUG
                    std::cout << "read error from server2 "  << std::endl;
#endif /*DEBUG*/
                    close(fd);
                    deleteEvent(fd, EPOLLIN);
                }
                else if(nread == 0){
                    close(fd);
                    deleteEvent(fd, EPOLLIN);
                }
                else{
#ifdef ACK
                    //TODO:add signal and mutex
//                    modifyEvent(fd, EPOLLOUT);
#endif/*ACK*/
                }
                bzero(&_Msg, sizeof(SBufferNode));

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

                //read from shmFromWorker
                SBufferNode *node =
                    _shmFromWorkerMap[fd]->svrRecvData();
                if(node == NULL){
#ifdef DEBUG
                    std::cout << "cannot recv data from worker"
                        << std::endl;
#endif /*DEBUG*/
                }

                //Write to sockconnector
                int server2fd = (node->svrProMsg).serverConnectFd;
                std::cout << "msg : "
                    << (node->cliMsg).msg
                    << std::endl;
                CUtil::writeMsgToSock(server2fd, node,
                        sizeof(SBufferNode));

            }
            else{//if the msg come from client
                //struct it and send it to server2
                int nread;
                if((nread = read(fd, buf, RECVMAXSIZE)) < 0){
#ifdef DEBUG
                    std::cout << "read error from client" << std::endl;
#endif /*DEBUG*/
                    close(fd);
                    deleteEvent(fd, EPOLLIN);
                }
                else if(nread == 0){//same as EPOLLRDHUP
                    close(fd);
                    deleteEvent(fd, EPOLLIN);
                }
                else{
                    //write to shmToWorker
                    SBufferNode tempMsg;
                    tempMsg.epollfd = _epollfd;
                    tempMsg.cliMsg.clientAcceptFd = fd;
                    bzero(tempMsg.cliMsg.msg, RECVMAXSIZE);
                    memcpy(tempMsg.cliMsg.msg, buf, nread);
                    tempMsg.cliMsg.length = nread;
                    tempMsg.svrProMsg.serverConnectFd
                        = SockConnector::_sockfd;
    //               tempMsg.svrProMsg.md5Result = true;
                    tempMsg.svrProMsg.serverMd5Result = true;
                    tempMsg.event = events[i];

                    _shmToWorkerMap[fd]->svrSendData(&tempMsg);



                    char msg[5] = "0000";
                    CUtil::writeMsgToFifo(_fifoMap[fd],
                            msg,
                            5);

#ifdef ACK
                    modifyEvent(fd, EPOLLOUT);
#endif /*ACK*/

                }
            }
        }

        else if(events[i].events & EPOLLOUT){
            //just come from client
            int nwrite;
            std::string ack = "7E457E";
            if((nwrite = write(fd, ack.c_str(),
                            ack.size())) < 0){
#ifdef DEBUG
                std::cout << "write error in epoll"
                    << std::endl;
#endif /*DEBUG*/
                deleteEvent(fd, EPOLLOUT);
                close(fd);
            }
            else{
                //TODO:limit flow module
                modifyEvent(fd, EPOLLIN);
            }
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



