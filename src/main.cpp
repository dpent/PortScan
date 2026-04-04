#include "sockets/headers/tcpSocket.h"
#include "sockets/headers/udpSocket.h"
#include <iostream>

int main(){

#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cout << "WSAStartup failed" << std::endl;
        return false;
    }
    else {
        std::cout<<"WSAStartup successful" << std::endl;
    }
#endif

    TCPSocket socket;
    if(socket.connectTo("127.0.0.1", 27017)){
        std::cout<<"Connected to port 27017"<<std::endl;
        
        socket.sendBytes((char*)"Hello world", 11);
        std::cout<<"Sent data"<<std::endl;

        socket.disconnect();
        std::cout<<"Disconnected"<<std::endl;
    }else{
        std::cout<<"Failed to connect"<<std::endl;
    }

    UDPSocket udpSocket;
        if(udpSocket.connectTo("127.0.0.54", 53)){
        std::cout<<"Connected to port 53"<<std::endl;

        udpSocket.sendBytes((char*)"Hello world", 11);
        std::cout<<"Sent data"<<std::endl;

        udpSocket.disconnect();
        std::cout<<"Disconnected"<<std::endl;
    }else{
        std::cout<<"Failed to connect"<<std::endl;
    }

    return 0;
}