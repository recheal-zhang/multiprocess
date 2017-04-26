#include <stdlib.h>

#include <iostream>
#include <string>

#include "Epoll.h"
#include "Util.h"
#include "Define.h"
#include "Fifo.h"

using namespace std;

int main(){
    cout << "hello master" << endl;

    Epoll epollController;

//    epollController.addFifofdFromClient(
//            CUtil::openfileReadonlyNonblock(
//                (CUtil::getFifoName(WORKER_1_KEY)).c_str()));

    std::string fifoClientName =
        CUtil::getFifoName(WORKER_1_KEY);
    int fifoFdFromClient =
        CUtil::openfileReadonlyNonblock(
                fifoClientName.c_str());

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

    epollController.addFifoFdFromClient(fifoFdFromClient);

    epollController.addFifoFdToClient(fifoFdFromClient,
            fifoFdToClient);

    epollController.monitor();

    return 0;
}
