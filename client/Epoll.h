#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <iostream>
#include <string>
#include <sys/epoll.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "DefineVal.h"
#include "Md5.h"

#define EPOLLEVENTS 100
#define FDSIZE 1000
#define BUFFER_SIZE 1024

struct timeval startval;
struct timeval endval;

//std::string filename = "../../../Downloads/victory.ts";
std::string filename = "Md5.h";
//std::string filename = "/home/riozhang/Downloads/cpp.zip";
void do_epoll(int connectfd, char *buf, FILE *fp);

void handle_events(int epollfd, struct epoll_event *events,
        int num, int listenfd, char *buf, FILE *fp);

void do_read(int epollfd, int fd, char *buf);

void do_write(int epollfd, int fd, char *buf, FILE *fp);

void add_event(int epollfd, int fd, int state);

void modify_event(int epollfd, int fd, int state);

void delete_event(int epollfd, int fd, int state);

void sendMsg(int sockfd, const void *buffer, size_t len, int flags){
    if(send(sockfd, buffer, len, flags) < 0){
#ifdef DEBUG
        std:: cout << "send msg error" << std::endl;
#endif
        exit(-1);
    }
}

void sendCmd(int sockfd, const void *buffer, size_t len, int flags){
    if(send(sockfd, buffer, len, flags) < 0){
#ifdef DEBUG
        std:: cout << "send cmd error" << std::endl;
#endif
        exit(-1);
    }
}

void do_epoll(int connectfd, char *buf, FILE *fp){
    int epollfd;
    struct epoll_event events[EPOLLEVENTS];

    epollfd = epoll_create(FDSIZE);

    add_event(epollfd, connectfd, EPOLLIN);

    gettimeofday(&startval, NULL);

    int ret;
    while(true){
        ret = epoll_wait(epollfd, events, EPOLLEVENTS, TIMEOUT);
        handle_events(epollfd, events, ret, connectfd, buf, fp);
    }
    close(epollfd);
}

void handle_events(int epollfd, struct epoll_event *events,
        int num, int listenfd, char *buf, FILE *fp){
    int i;
    int fd;
    for(i = 0; i < num; i++){
        fd = events[i].data.fd;
        if(events[i].events & EPOLLIN){
            do_read(epollfd, fd, buf);
        }
        else if(events[i].events & EPOLLOUT){
            do_write(epollfd, fd, buf, fp);
        }
    }
}

void do_read(int epollfd, int fd, char *buf){
    bzero(buf, BUFFER_SIZE);
    int nread;
    if((nread = read(fd, buf, BUFFER_SIZE)) < 0){
#ifdef DEBUG
        std::cout << "read error in epoll" << std::endl;
#endif /*DEBUG*/
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    }
    else if(nread == 0){
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    }
    else{
        modify_event(epollfd, fd, EPOLLOUT);
    }
}

void do_write(int epollfd, int fd, char *buf, FILE *fp){
    bzero(buf, sizeof(buf));

    int fileBlockLen = fread(buf, sizeof(char),
            BUFFER_SIZE, fp);
    if(fileBlockLen < 0){
#ifdef DEBUG
        std::cout << "fread error in epoll" << std::endl;
#endif /*DEBUG*/
        gettimeofday(&endval, NULL);
        double time = static_cast<double>(endval.tv_usec - startval.tv_usec)/1000000
            + (endval.tv_sec - startval.tv_sec);
        cout << "time cost : " << time << endl;


        std::string endFileMsg = "7E4511117E";
        sendCmd(fd, endFileMsg.c_str(), endFileMsg.size(), 0);
        sendCmd(fd, endFileMsg.c_str(), endFileMsg.size(), 0);
        fclose(fp);
        close(fd);
        std::cout << "makefile Md5 = " << md5file(filename.c_str()) << std::endl;
        exit(0);

    }
    else if(fileBlockLen == 0){
#ifdef DEBUG
        std::cout << "fread end in epoll" << std::endl;
#endif /*DEBUG*/
        gettimeofday(&endval, NULL);
        double time = static_cast<double>(endval.tv_usec - startval.tv_usec)/1000000
            + (endval.tv_sec - startval.tv_sec);
        cout << "time cost : " << time << endl;

        std::string endFileMsg = "7E4511117E";
        sendCmd(fd, endFileMsg.c_str(), endFileMsg.size(), 0);
        sendCmd(fd, endFileMsg.c_str(), endFileMsg.size(), 0);

        fclose(fp);
        close(fd);
        std::cout << "makefile Md5 = " << md5file(filename.c_str()) << std::endl;
        exit(0);
    }
    else{
#ifdef DELAY
        usleep(10);
#endif /*DELAY*/
        sendMsg(fd, buf, fileBlockLen, 0);
        modify_event(epollfd, fd, EPOLLIN);
    }

}

void add_event(int epollfd, int fd, int state){
    struct epoll_event ev;
    ev.events = state | EPOLLET;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void delete_event(int epollfd, int fd, int state){
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

void modify_event(int epollfd, int fd, int state){
    struct epoll_event ev;
    ev.events = state | EPOLLET;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}


#endif /*_EPOLL_H_*/
