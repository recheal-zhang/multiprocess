#include <iostream>
#include <string>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include <fcntl.h>

#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "DefineVal.h"
#include "Util.h"
#include "Md5.h"

#define IPADRESS "127.0.0.1"
#define PORT 126
#define MAXSIZE 1024
#define LISTENQ 5
#define FDSIZE 1000
#define EPOLLEVENTS 100

long long queryNum = 0;
pthread_mutex_t queryNumMutexLock = PTHREAD_MUTEX_INITIALIZER;
struct timeval startval;
struct timeval endval;
int cost = 1;

int gFileFd[EPOLLEVENTS];
FILE *gFile[EPOLLEVENTS] = {0};

void *show_msg_thread(void *){
    std::cout << "come in" << std::endl;
    while(true){
//       gettimeofday(&endval, NULL);
//       long long cost = (endval.tv_sec - startval.tv_sec) * 1000000 + (endval.tv_usec - startval.tv_usec);
       pthread_mutex_lock(&queryNumMutexLock);
       int qps = static_cast<int>(queryNum / (2));
       queryNum = 0;
       pthread_mutex_unlock(&queryNumMutexLock);
       std::cout << "qps = " << qps << std::endl;
        sleep(2);
        if(cost == 1){cost = 2;}
        else{cost += 2;}

    }
}


void add_query_num(){
    pthread_mutex_lock(&queryNumMutexLock);
    queryNum++;
    pthread_mutex_unlock(&queryNumMutexLock);
}

//function declaration
int socket_bind(const char *ip, int port);

void do_epoll(int listenfd);

void sherror(const char *err);

void handle_events(int epollfd, struct epoll_event *events, int num, int listenfd, threadMsg *buf);

void handle_accept(int epollfd, int listenfd);

void do_read(int epollfd, int fd, threadMsg *buf);

void do_write(int epollfd, int fd, threadMsg *buf);

void add_event(int epollfd, int fd, int state);

void modify_event(int epollfd, int fd, int state);

void delete_event(int epollfd, int fd, int state);

int main(int argc, char **argv){
    int listenfd;
//    pthread_t showMsgThreadId;
//    pthread_create(&showMsgThreadId, NULL, show_msg_thread, NULL);
    listenfd = socket_bind(IPADRESS, PORT);
    listen(listenfd, LISTENQ);
//    gettimeofday(&startval, NULL);
    do_epoll(listenfd);
    return 0;
}

int socket_bind(const char *ip, int port){
    int listenfd;
    struct sockaddr_in servaddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << listenfd << std::endl;
    if(listenfd == -1){
        sherror("socket error");
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &servaddr.sin_addr);
    servaddr.sin_port = htons(port);
    if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1){
        sherror("bind error");
    }
    Util::setReuseAddr(listenfd);
    return listenfd;
}

void do_epoll(int listenfd){
    int epollfd;
    struct epoll_event events[EPOLLEVENTS];
    int ret;
    char buf[MAXSIZE];
    threadMsg msg;
    memset(buf, 0, MAXSIZE);

    epollfd = epoll_create(FDSIZE);

    add_event(epollfd, listenfd, EPOLLIN);

    while(true){
        ret = epoll_wait(epollfd, events, EPOLLEVENTS, 500);
        handle_events(epollfd, events, ret, listenfd, &msg);
    }

    close(epollfd);
}


void handle_events(int epollfd, struct epoll_event *events,
        int num, int listenfd, threadMsg *buf){
    int i;
    int fd;

    for(i = 0; i < num; i++){
        fd = events[i].data.fd;

        if((fd == listenfd) && (events[i].events & EPOLLIN)){
//            add_query_num();
            handle_accept(epollfd, listenfd);
        }
        else if(events[i].events & EPOLLIN){
            //TODO:multithread
//            add_query_num();
            do_read(epollfd, fd, buf);
        }
        else if(events[i].events & EPOLLOUT){
            //TODO:multithread
            do_write(epollfd, fd, buf);
        }
    }
}

void handle_accept(int epollfd, int listenfd){
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;
    clifd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddrlen);
    std::cout << "server 1 connect" << std::endl;
    Util::setNonblock(clifd);
    Util::setNoDelay(clifd);
    Util::setReuseAddr(clifd);
    if(clifd == -1){
        std::cout << "accept error" << std::endl;
    }
    //TODO:get client
    else{
        add_event(epollfd, clifd, EPOLLIN);
    }
}

void do_read(int epollfd, int fd, threadMsg *buf){
    int nread;
    nread = read(fd, buf, sizeof(threadMsg));
    if(nread == -1){
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
        std::cout << "read error" << std::endl;
    }
    else if(nread == 0){
        std::cout << "client close" << std::endl;
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    }
    else{
//        char bufMsg[buf->cliMsg.length];
//        memcpy(bufMsg, buf->cliMsg.msg, buf->cliMsg.length);
        std::string tempMsg = std::string(buf->cliMsg.msg);

        threadMsg ret;
        ret.cliMsg.clientAcceptFd = buf->cliMsg.clientAcceptFd;
        ret.cliMsg.length = 0;
        ret.svrProMsg.serverConnectFd = buf->svrProMsg.serverConnectFd;
        ret.event = buf->event;
        std::size_t found = tempMsg.find("7E4500007E");
        if(found != std::string::npos){ //touch file cmd

//        string start("7E4500007E");
//        if(tempMsg == start){
            char buffer[10];
            snprintf(buffer, 10, "%d",
                    buf->cliMsg.clientAcceptFd);
            std::string fdStr = std::string(buffer);
            std::string filename = fdStr + ".temp";
            if(gFile[buf->cliMsg.clientAcceptFd] != 0){
                fclose(gFile[buf->cliMsg.clientAcceptFd]);
                gFile[buf->cliMsg.clientAcceptFd] = 0;
            }
            gFile[buf->cliMsg.clientAcceptFd] = fopen(
                    filename.c_str(), "w");
            gFileFd[buf->cliMsg.clientAcceptFd] = open(
                    filename.c_str(),
                    O_WRONLY | O_CREAT | O_TRUNC,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            std::cout << "openfile" << std::endl;
            ret.svrProMsg.serverMd5Result = true;
        }
        else if((found = tempMsg.find("7E4511117E"))
                != std::string::npos){
            std::cout << "closefile" << std::endl;
            if(gFile[buf->cliMsg.clientAcceptFd] != 0){
                fclose(gFile[buf->cliMsg.clientAcceptFd]);
                gFile[buf->cliMsg.clientAcceptFd] = 0;

                char buffer[10];
                snprintf(buffer, 10, "%d",
                        buf->cliMsg.clientAcceptFd);
                std::string fdStr = std::string(buffer);
                std::string filename = fdStr + ".temp";
                std::cout << "file's Md5 = " <<
                    md5file(filename.c_str()) << std::endl;
                ret.svrProMsg.serverMd5Result = true;
            }
        }
        else{
//            std::cout << buf->cliMsg.msg;
            if(gFile[buf->cliMsg.clientAcceptFd] !=0){
                Util::writeMsgToFile(gFile[buf->cliMsg.clientAcceptFd],
                        buf->cliMsg.msg,
                        buf->cliMsg.length);

                ret.svrProMsg.serverMd5Result = true;
            }
            else{
                std::cout << "gFile not create" << std::endl;
                ret.svrProMsg.serverMd5Result = false;
            }
        }
//        modify_event(epollfd, fd, EPOLLOUT);
        write(fd, &ret, sizeof(threadMsg));
    }
}

void do_write(int epollfd, int fd, threadMsg *buf){
    int nwrite;
    std::string ack = "7E457E";
    bzero(buf->cliMsg.msg, RECVMAXSIZE);
    memcpy(buf->cliMsg.msg, ack.c_str(), ack.size());
    nwrite = write(fd, buf, sizeof(buf));
    if(nwrite ==-1){
        std::cout << "write error" << std::endl;
        close(fd);
        delete_event(epollfd, fd, EPOLLOUT);
    }
    else{
        modify_event(epollfd, fd, EPOLLIN);
    }
}

void add_event(int epollfd, int fd, int state){
    struct epoll_event ev;
    ev.events = state;
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
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}

void sherror(const char *err){
#ifdef DEBUG
    std::cout << std::string(err) << std::endl;
#endif
    exit(-1);
}
