#pragma once
#include "psServerSocket.h"

class UDPServerSocket : public psServerSocket{

    public:
    UDPServerSocket(int port);
    bool start() override;
    void stop() override;
};