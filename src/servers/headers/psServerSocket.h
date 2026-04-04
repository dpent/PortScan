#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#endif

class psServerSocket{
    protected:
        #ifdef _WIN32
            SOCKET s;
        #else
            int s;
        #endif

        int port;
        sockaddr_in addr;

    public:
    psServerSocket(int port);
    virtual bool start();
    virtual void stop();
};