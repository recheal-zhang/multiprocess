/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#ifndef _INETADDR_H_
#define _INETADDR_H_

#include <string>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>


class INETAddr{
    public:
        INETAddr(const char *addr, const short &port);
        ~INETAddr();
        short getPort() const;
        std::string getIPAddr() const;
        struct sockaddr *getAddr() const;
        size_t getSize() const;

    private:
        sockaddr_in _addr;
};

#endif /*_INETADDR_H_*/
