#ifndef __UDPSocket_H__
#define __UDPSocket_H__


#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <iostream>
#include "Types.h"


class UDPSocket
{
    UDPSocket();

public:
    ~UDPSocket();

    static UDPSocket & instance();
    void Initial(const char *host, int port);

    int Receive(char *msg) ;
    int Send(const char *msg);

private:
    bool        mIsInitialOK;
    sockaddr_in mAddress;

#ifdef WIN32
    SOCKET mSockfd;
#else
    int mSockfd;
#endif

};


#endif

