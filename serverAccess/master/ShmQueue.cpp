/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#include <string.h>
#include "ShmQueue.h"
#include "Define.h"

CShmQueue::CShmQueue() :
    front(0),
    rear(0)
{
    memset(_queue,
            0,
            sizeof(SBufferNode) * MAX_QUEUE_NODE_COUNT);
}

CShmQueue::~CShmQueue(){}

bool CShmQueue::isEmpty(){
    return (rear == front);
}

bool CShmQueue::isFull(){
    return ((rear + 1) % MAX_QUEUE_NODE_COUNT
            == front);
}

bool CShmQueue::enqueue(SBufferNode *node){
    if(isFull()){
        return false;
    }
    memcpy(&_queue[rear],
            node,
            sizeof(SBufferNode));

    rear = (rear + 1) % MAX_QUEUE_NODE_COUNT;
    return true;
}

SBufferNode *CShmQueue::dequeue(){
    if(isEmpty()){
        return NULL;
    }
    SBufferNode *node = &_queue[front];
    front = (front + 1) % MAX_QUEUE_NODE_COUNT;
    return node;
}
