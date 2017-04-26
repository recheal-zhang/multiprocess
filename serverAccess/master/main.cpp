#include <stdlib.h>

#include <iostream>
#include <string>

#include "Epoll.h"
#include "Util.h"
#include "Define.h"
#include "DefineVal.h"
#include "Fifo.h"
#include "ShareMemory.h"
#include "ShmQueue.h"
#include "SockConnector.h"
#include "SOCKAcceptor.h"
#include "INETAddr.h"


using namespace std;

int main(){
    cout << "hello master" << endl;

    Epoll epollController;

    //master: get worker's fifo info
    std::string fifoClientName =
        CUtil::getFifoName(WORKER_1_KEY);
    int fifoFdFromClient =
        CUtil::openfileReadonlyNonblock(
                fifoClientName.c_str());

    //master: mkfifo to worker & open fifo
    std::string fifoServerName =
        CUtil::getFifoName(MASTER_1_KEY);
    CFifo fifoSend(fifoServerName.c_str());
    int fifoFdToClient =
        open(fifoServerName.c_str(),
                O_WRONLY,
                0);
    if(fifoFdToClient == -1){
#ifdef DEBUG
        std::cout << "open file error" << std::endl;
        exit(EXIT_FAILURE);
#endif/*DEBUG*/
    }

    //master: create share memory to worker
    CShareMemory shmToWorker;
    shmToWorker.createShareMem();

    //master: create share memory from worker
    CShareMemory shmFromWorker;
    shmFromWorker.createShareMem();

    //sock connect with server2
    SockConnector *sockConnectorInstance =
        SockConnector::getInstance();
    sockConnectorInstance->sockConnect(
            SVRADDRESS,
            SVRPORT);

    //sock listen & accept
    INETAddr inetAddr(IPADDRESS, PORT);
    SOCKAcceptor sockAcceptor;
    sockAcceptor.sockBindListen();

    //epoll add info
    epollController.addFifoFdFromClient(fifoFdFromClient);
    epollController.addFifoFdToClient(fifoFdFromClient,
            fifoFdToClient);
    epollController.addShmToWorkerInfo(fifoFdFromClient,
            &shmToWorker);
    epollController.addShmFromWorkerInfo(fifoFdFromClient,
            &shmFromWorker);
    epollController.getSockAcceptorInfo(&sockAcceptor);
    epollController.getSockConnectorInfo(sockConnectorInstance);

    epollController.monitor();

    return 0;
}
