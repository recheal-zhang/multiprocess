/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#ifndef _SOCKCONNECTOR_H_
#define _SOCKCONNECTOR_H_

#include "MutexLock.h"

class SockConnector{
    public:
        static SockConnector *getInstance();
        void sockConnect(const char *ip, const short &port);

        static int _sockfd;
    private:
        SockConnector(){}
        virtual ~SockConnector(){}
        static MutexLock _mutex;
        static SockConnector *_sockConnector;
};

#endif /*_SOCKCONNECTOR_H_*/
