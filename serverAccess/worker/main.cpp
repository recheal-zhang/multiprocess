#include <unistd.h>
#include <string.h>

#include <iostream>
#include <string>

#include "Util.h"
#include "Define.h"
#include "Fifo.h"
#include "Epoll.h"
#include "ShareMemory.h"
#include "ShmQueue.h"

using namespace std;

int main(){
    cout << "hello worker" << endl;
    //create epoll
    Epoll workerEpoll;

    //worker: mkfifo to master & open fifo
    std::string fifoClientName =
        CUtil::getFifoName(WORKER_1_KEY);
    CFifo fifoSend(fifoClientName.c_str());
    int fd = open(fifoClientName.c_str(),
                    O_WRONLY,
                    0);

    //worker: get server's fifo info
    std::string fifoServerName =
        CUtil::getFifoName(MASTER_1_KEY);
    workerEpoll.addFifoWriteFdInfo(fd);
    workerEpoll.addFifoFdFromServer(
            CUtil::openfileReadonlyNonblock(
                fifoServerName.c_str()));

    //worker: create share memory to master
    CShareMemory shmToMaster;
    shmToMaster.createShareMem();
    workerEpoll.addShmToMasterInfo(&shmToMaster);

    //worker: create share memory from master
    CShareMemory shmFromMaster;
    shmFromMaster.createShareMem();
    workerEpoll.addShmFromMasterInfo(&shmFromMaster);


    workerEpoll.monitor();
    return 0;
}
