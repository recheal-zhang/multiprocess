/*
 * Copyright (C) riozhang
 * Copyright (C) tencent, Inc.
 * */

#include "INETAddr.h"
#include "DefineVal.h"

INETAddr::INETAddr(const char *addr,const short &port){
    //set up the address to become a server.
    memset(reinterpret_cast<void *>(&_addr),
            0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_port = port;
    inet_pton(AF_INET, addr, &(_addr.sin_addr));
}

INETAddr::~INETAddr(){}

short INETAddr::getPort() const{
    return _addr.sin_port;
}

std::string INETAddr::getIPAddr() const{
    char str[INET_ADDRSTRLEN];
    if(inet_ntop(AF_INET, &(_addr.sin_addr),
                str, INET_ADDRSTRLEN) == NULL){
        return NULL;
    }
    return std::string(str);
}

struct sockaddr *INETAddr::getAddr() const{
    return (struct sockaddr*)(&_addr);
}


size_t INETAddr::getSize() const {
    return sizeof(_addr);
}
