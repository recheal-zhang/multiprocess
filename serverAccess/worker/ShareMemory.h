/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#ifndef _SHAREMEMORY_H_
#define _SHAREMEMORY_H_

#include "BufferManager.h"
#include "ShmQueue.h"

class CShareMemory{
    public:
        CShareMemory();
        ~CShareMemory();

        bool createShareMem();
        bool deleteShareMem();
        bool ctlDeleteShareMem();

        bool svrSendData(SBufferNode *);
        SBufferNode *svrRecvData();

        bool workerSendData(SBufferNode *);
        SBufferNode *workerRecvData();

        int getShmid();
    private:
        CBufferManager *_bufferManager;

        int _bufferID;
};

#endif /*_SHAREMEMORY_H_*/
