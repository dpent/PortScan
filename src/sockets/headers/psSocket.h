#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#endif

class psSocket{ // Base socket class. Is the OS agnostic interface.

    protected:
        #ifdef _WIN32
            SOCKET s;
        #else
            int s;
        #endif

        char* ip;
        int port;
        sockaddr_in addr;

    public:

        psSocket();
        virtual bool connectTo(const char* ip, int port);
        virtual void disconnect();
        virtual bool sendBytes(char* buffer, int length);
        virtual char* receiveBytes();
    
};