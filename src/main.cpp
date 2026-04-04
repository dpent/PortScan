#include "sockets/headers/tcpSocket.h"
#include "servers/headers/tcpServerSocket.h"
#include "sockets/headers/udpSocket.h"
#include "servers/headers/udpServerSocket.h"
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
    if(socket.connectTo("127.0.0.1", 8080)){
        std::cout<<"Connected to port 8080"<<std::endl;
        
        socket.sendBytes((char*)"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n", 36);
        std::cout<<"Sent data"<<std::endl;

        char* response = socket.receiveBytes();
        if(response){
            std::cout<<"Received response: "<<response<<std::endl;
            delete[] response;
        }

        socket.disconnect();
        std::cout<<"Disconnected"<<std::endl;
    }else{
        std::cout<<"Failed to connect"<<std::endl;
    }

    UDPSocket udpSocket;
    if(udpSocket.connectTo("127.0.0.1", 8080)){
        std::cout<<"Connected to port 8080"<<std::endl;

        udpSocket.sendBytes((char*)"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n", 36);
        std::cout<<"Sent data"<<std::endl;

        char* response = udpSocket.receiveBytes();
        if(response){
            std::cout<<"Received response: "<<response<<std::endl;
            delete[] response;
        }

        udpSocket.disconnect();
        std::cout<<"Disconnected"<<std::endl;
    }else{
        std::cout<<"Failed to connect"<<std::endl;
    }

    return 0;
}