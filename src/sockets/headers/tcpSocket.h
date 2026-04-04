#pragma once
#include "psSocket.h"


class TCPSocket : public psSocket{

    public:
        TCPSocket();
        bool connectTo(const char* ip, int port) override;
        void disconnect() override;
        bool sendBytes(char* buffer, int length) override;
        Response receiveBytes() override;
        std::string scanPort(const char* ip, int port) override;
};