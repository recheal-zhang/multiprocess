/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include <iostream>
#include <string>

#include "Fifo.h"

CFifo::CFifo(const char *pathName) :
    _pathName(pathName){
    if((mkfifo(_pathName, O_CREAT | O_EXCL) < 0)
            && (errno != EEXIST)){
#ifdef DEBUG
        std::cout << "cannot mkfifo" << std::endl;
#endif /*DEBUG*/
        exit(EXIT_FAILURE);
    }
}

CFifo::~CFifo(){
    unlink(_pathName);
}
