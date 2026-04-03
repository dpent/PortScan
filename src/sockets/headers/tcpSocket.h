#pragma once
#include "psSocket.h"


class TCPSocket : public psSocket{

    public:
        TCPSocket();
        bool connectTo(const char* ip, int port) override;
        void disconnect() override;
};