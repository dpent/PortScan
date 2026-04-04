#include "../headers/tcpServerSocket.h"

TCPServerSocket::TCPServerSocket(int port) : psServerSocket(port){
    return;
}

bool TCPServerSocket::start(){
    s = socket(AF_INET, SOCK_STREAM, 0);
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

    if (listen(s, 5) < 0) {
        return false;
    }

    return true;
}

void TCPServerSocket::stop(){
    #ifdef _WIN32
    closesocket(s);
    #else
    close(s);
    #endif
}