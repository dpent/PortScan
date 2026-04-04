#include "../headers/udpServerSocket.h"

UDPServerSocket::UDPServerSocket(int port) : psServerSocket(port){
    return;
}

bool UDPServerSocket::start(){
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(s, (sockaddr*)&addr, sizeof(addr)) < 0) {
        return false;
    }

    return true;
}

void UDPServerSocket::stop(){
    #ifdef _WIN32
    closesocket(s);
    #else
    close(s);
    #endif
}