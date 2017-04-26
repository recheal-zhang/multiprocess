/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <sstream>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>


class Util{
    public:
        //TODO: static xx yyy(){}
        //<<< Util::yyy();
        static void setNonblock(int fd){
            int flags = fcntl(fd, F_GETFL, 0);
            fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        }

        static void setNoDelay(int sockfd){
            int optval;
            setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,
                    &optval, sizeof(optval));
        }

        static void setReuseAddr(int sockfd){
            int optval;
            setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                    &optval, sizeof(optval));
        }

        static std::string writeLog(const char *logMsg,
                const char *file,
                const int &line,
                const char *date,
                const char *timer ){
            std::stringstream ss;
            ss << "[TIME]:" << timer << "\t"
                << logMsg << "\t"
                << "[FILE]:" << file << "\t"
                << "[LINE]:" << line << "\n";
            return ss.str();
        }

        static void writeMsgToSock(int fd, void *buf, size_t len){
            int nwrite;
            if((nwrite = write(fd, buf, len)) == -1){
#ifdef DEBUG
                std::cout << "write error in Util" << std::endl;
#endif /*DEBUG*/
                close(fd);
            }
        }

        static void readMsgFromSock(int fd, void *buf, size_t len){
            int nread;
            if((nread = read(fd, buf, len)) < 0){
#ifdef DEBUG
                std::cout << "read error in Util" << std::endl;
#endif /*DEBUG*/
                close(fd);
            }
        }

        static void writeMsgToFile(FILE *fp, const void *buf, size_t len){
            int nwrite;
#ifdef SHOWMSG
            std::cout << std::string(static_cast<const char*>(buf)) <<std::endl;
#endif /*SHOWMSG*/
            if((nwrite = fwrite(buf, sizeof(char), len, fp)) < 0){
#ifdef DEBUG
                std::cout << "write error in Util's writeFile" << std::endl;
#endif /*DEBUG*/
                fclose(fp);
            }

        }

};

#endif /*_UTIL_H_*/
