#include "../headers/udpSocket.h"

UDPSocket::UDPSocket() : psSocket(){
    return;
}

bool UDPSocket::connectTo(const char* ip, int port){
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    #ifdef _WIN32
    addr.sin_addr.s_addr = inet_pton(AF_INET, ip, &(addr.sin_addr));
    #else
    inet_pton(AF_INET, ip, &addr.sin_addr);
    #endif
    return ::connect(s, (sockaddr*)&addr, sizeof(addr)) == 0;
}

void UDPSocket::disconnect(){
    #ifdef _WIN32
    closesocket(s);
    #else
    close(s);
    #endif
}