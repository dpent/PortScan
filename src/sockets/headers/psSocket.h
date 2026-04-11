#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <cstring>
#include <random>

#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
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
        static std::vector<uint8_t> buildTFTPRequest(const std::string& filename);
        static std::vector<uint8_t> buildTelnetProbe();;
        static std::vector<uint8_t> buildDHCPDiscover(uint32_t xid, const std::vector<uint8_t> mac);
        static bool isValidTFTP(Response& response);
        static bool isValidDNS(Response& res);
        static bool isValidNTP(Response& res);
        static bool isValidTelnet(Response& res);
        static bool isValidDHCP(Response& res);
        static uint32_t getXid();
        static std::vector<uint8_t> getRandomMac();
        static std::string getBroadcastAddress();

        virtual bool connectTo(const char* ip, int port);
        virtual void disconnect();
        virtual bool sendBytes(char* buffer, int length);
        virtual Response receiveBytes();
        virtual std::string scanPort(const char* ip, int port);
    
};