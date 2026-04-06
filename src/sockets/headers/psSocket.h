#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>

#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#endif

struct Response{
    char* data;
    #ifdef _WIN32
    int length;
    #else
    ssize_t length;
    #endif
};

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
        static std::string analyseHTTP(std::string response);
        static std::string analyseHTTPS(std::string response);
        static std::string analyseBanner(std::string response);
        static std::vector<uint8_t> buildDNSQuery(std::string domain);
        static std::vector<uint8_t> buildNTPQuery();
        static std::vector<uint8_t> buildSNMPQuery();
        static std::vector<uint8_t> buildTLSClientHello();
        virtual bool connectTo(const char* ip, int port);
        virtual void disconnect();
        virtual bool sendBytes(char* buffer, int length);
        virtual Response receiveBytes();
        virtual std::string scanPort(const char* ip, int port);
    
};