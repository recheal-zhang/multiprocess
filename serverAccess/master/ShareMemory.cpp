/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <iostream>

#include "ShareMemory.h"

CShareMemory::CShareMemory() :
    _bufferManager(0),
    _bufferID(0)
{}

CShareMemory::~CShareMemory(){}

bool CShareMemory::createShareMem(){
    _bufferID = shmget((key_t)WORKER_1_KEY,
            sizeof(CBufferManager),
            0666 | IPC_CREAT);

    if(_bufferID == -1){
#ifdef DEBUG
        std::cout << "shmget failed"
            << std::endl;
#endif /*DEBUG*/
        exit(EXIT_FAILURE);
    }

    void *sharedMemory = (void *)0;
    sharedMemory = shmat(_bufferID, (void *)0, 0);
    if(sharedMemory == (void *)(-1)
            || sharedMemory == NULL){
#ifdef DEBUG
        std::cout << "shmat failed" << std::endl;
#endif /*DEBUG*/
        exit(EXIT_FAILURE);
    }

    //bind CBufferManager
    _bufferManager = (CBufferManager *)sharedMemory;
    return true;
}

bool CShareMemory::deleteShareMem(){
    if((shmdt(_bufferManager)) == -1){
#ifdef DEBUG
        std::cout << "shmdt failed" << std::endl;
#endif /*DEBUG*/
        exit(EXIT_FAILURE);
    }

    return true;
}

bool CShareMemory::ctlDeleteShareMem(){
    if((shmctl(_bufferID, IPC_RMID, 0)) == -1){
#ifdef DEBUG
        std::cout << "shmctl(IPC_RMID) failed"
            << std::endl;
#endif /*DEBUG*/
        exit(EXIT_FAILURE);
    }
    return true;
}

bool CShareMemory::svrSendData(SBufferNode *node){
    if(node == NULL ||
            _bufferManager == NULL){
#ifdef DEBUG
        std::cout << "node is NULL" << std::endl;
#endif /*DEBUG*/
        return false;
    }
    _bufferManager->_svrSendQueue.enqueue(node);

    return true;
}

SBufferNode *CShareMemory::svrRecvData(){
    if(_bufferManager == NULL){
        return NULL;
    }

    SBufferNode *node =
        _bufferManager->_svrRecvQueue.dequeue();
    return node;
}

bool CShareMemory::workerSendData(SBufferNode *node){
    if(node == NULL ||
            _bufferManager == NULL){
        return false;
    }

    _bufferManager->_svrRecvQueue.enqueue(node);
    return true;
}

SBufferNode *CShareMemory::workerRecvData(){
    if(_bufferManager == NULL){
        return NULL;
    }

    SBufferNode *node =
        _bufferManager->_svrSendQueue.dequeue();
    return node;
}
