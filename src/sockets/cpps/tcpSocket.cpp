#include "../headers/tcpSocket.h"
#include <iostream>

TCPSocket::TCPSocket() : psSocket(){
    return;
}

bool TCPSocket::connectTo(const char* ip, int port){
    s = socket(AF_INET, SOCK_STREAM, 0);
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

void TCPSocket::disconnect(){
    #ifdef _WIN32
    closesocket(s);
    #else
    close(s);
    #endif
}

bool TCPSocket::sendBytes(char* buffer, int length){

    #ifdef _WIN32
    int bytesSent = send(s, buffer, length, 0);
    #else
    ssize_t bytesSent = send(s, buffer, length, 0);
    #endif
    return bytesSent == length;
}

char* TCPSocket::receiveBytes(){
    char* buffer = new char[1024];
    #ifdef _WIN32
    int bytesReceived = recv(s, buffer, 1024, 0);
    #else
    ssize_t bytesReceived = recv(s, buffer, 1024, 0);
    #endif
    if(bytesReceived < 0){
        delete[] buffer;
        return nullptr;
    }

    return buffer;
}