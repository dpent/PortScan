#pragma once
#include "psSocket.h"


class UDPSocket : public psSocket{

    public:
        UDPSocket();
        bool connectTo(const char* ip, int port) override;
        void disconnect() override;
        bool sendBytes(char* buffer, int length) override;
        char* receiveBytes() override;
        std::string scanPort(const char* ip, int port) override;
};