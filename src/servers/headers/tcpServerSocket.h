#pragma once
#include "psServerSocket.h"

class TCPServerSocket : public psServerSocket{

    public:
    TCPServerSocket(int port);
    bool start() override;
    void stop() override;
};