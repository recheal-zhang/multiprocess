/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_

#include "ShmQueue.h"

class CShareMemory;

class CBufferManager{
        friend class CShareMemory;
    public:
        CBufferManager();
        ~CBufferManager(){}

    private:
        CShmQueue _svrSendQueue;
        CShmQueue _svrRecvQueue;
};

#endif /*_BUFFERMANAGER_H_*/
