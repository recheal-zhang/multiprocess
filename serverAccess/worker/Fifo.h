/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#ifndef _FIFO_H_
#define _FIFO_H_

class CFifo{
    public:
        CFifo(const char *);
        ~CFifo();


    private:
        const char *_pathName;
};

#endif /*_FIFO_H_*/
