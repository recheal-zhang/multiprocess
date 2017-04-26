/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#ifndef _SHMQUEUE_H_
#define _SHMQUEUE_H_

#include "Define.h"

struct SBufferNode{
    short dataLen;
    char data[MAX_NODE_DATA_SIZE];
};


class CShmQueue{
    public:
        CShmQueue();
        ~CShmQueue();

        bool isEmpty();
        bool isFull();
        bool enqueue(SBufferNode *);
        SBufferNode *dequeue();

    private:
        int front;
        int rear;
        SBufferNode _queue[MAX_QUEUE_NODE_COUNT];
};

#endif /*_SHGMQUEUE_H_*/
