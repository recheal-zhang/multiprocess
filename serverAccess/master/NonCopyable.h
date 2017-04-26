/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

class NonCopyable{
    protected:
        NonCopyable(){}
        virtual ~NonCopyable(){}

    private:
        NonCopyable(const NonCopyable&){}
        NonCopyable &operator=(const NonCopyable &){
            return *this;
        }
};

#endif /*_NONCOPYABLE_H_*/
