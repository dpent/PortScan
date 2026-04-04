#include "../headers/udpSocket.h"

UDPSocket::UDPSocket() : psSocket(){
    return;
}

bool UDPSocket::connectTo(const char* ip, int port){
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) return false;

    this->ip = (char*)ip;
    this->port = port;

    addr = {};
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
    shutdown(s, SD_BOTH);
    closesocket(s);
    #else
    shutdown(s, SHUT_RDWR);
    close(s);
    #endif
}

bool UDPSocket::sendBytes(char* buffer, int length){

    #ifdef _WIN32
    int bytesSent = sendto(s, buffer, length, 0, (sockaddr*)&addr, sizeof(addr));
    #else
    ssize_t bytesSent = sendto(s, buffer, length, 0, (sockaddr*)&addr, sizeof(addr));
    #endif
    return bytesSent == length;
}

char* UDPSocket::receiveBytes(){
    char* buffer = new char[1024];
    socklen_t addrLen = sizeof(addr);
    
    #ifdef _WIN32
    int bytesReceived = recvfrom(s, buffer, 1024, 0, (sockaddr*)&addr, &addrLen);
    #else   
    ssize_t bytesReceived = recvfrom(s, buffer, 1024, 0, (sockaddr*)&addr, &addrLen);
    #endif
    if(bytesReceived < 0){
        delete[] buffer;
        return nullptr;
    }

    return buffer;
}