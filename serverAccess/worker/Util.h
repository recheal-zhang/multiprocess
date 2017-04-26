/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include "Define.h"

class CUtil{
    public:
        static std::string getFifoName(int id){
            std::stringstream ss;
            ss << FIFO_PATH << id ;
            return ss.str();
        }

        static bool findFifoFdFromVec(
                std::vector<int> fifoFdVec,
                int fifoFd
                ){
            std::vector<int>::iterator it;
            it = find(fifoFdVec.begin(),
                    fifoFdVec.end(),
                    fifoFd);
            if(it != fifoFdVec.end()){
                return true;
            }
            return false;
        }
        static int openfileWriteonlyNonblock(
                const char *fileName
                ){
            int fd = open(fileName,
                    O_WRONLY | O_NONBLOCK,
                    0);
            return fd;
        }

        static int openfileReadonlyNonblock(
                const char *fileName
                ){
            int fd = open(fileName,
                    O_RDONLY | O_NONBLOCK,
                    0);
            return fd;
        }

        static void setNonblock(int fd){
            int flags = fcntl(fd, F_GETFL, 0);
            fcntl(fd, F_SETFL, flags | O_NONBLOCK);
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

        static void writeMsgToSock(int sockfd, void *buf, size_t len){
            int nwrite;
            if((nwrite = write(sockfd, buf, len)) == -1){
#ifdef DEBUG
                std::cout << "write error in Util" << std::endl;
#endif /*DEBUG*/
            }
        }

        static void writeMsgToFifo(int fifofd, void *buf, size_t len){
            int nwrite;
            if((nwrite = write(fifofd, buf, len)) == -1){
#ifdef DEBUG
                std::cout << "write error in Util" << std::endl;
#endif /*DEBUG*/
            }
        }


        static void readMsgFromFifo(int fifoFd,
                void *buf,
                size_t len){
            int nread;
            if((nread = read(fifoFd, buf, len)) == -1){
#ifdef DEBUG
                std::cout << "read error in Util" << std::endl;
#endif /*DEBUG*/
            }
        }

};

#endif /*UTIL_H_*/
