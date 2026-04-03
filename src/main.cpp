#include "sockets/headers/tcpSocket.h"
#include "sockets/headers/udpSocket.h"
#include <iostream>

int main(){

    TCPSocket socket;
    if(socket.connectTo("127.0.0.1", 27017)){
        std::cout<<"Connected to port 27017"<<std::endl;
        socket.disconnect();
        std::cout<<"Disconnected"<<std::endl;
    }else{
        std::cout<<"Failed to connect"<<std::endl;
    }

    UDPSocket udpSocket;
        if(udpSocket.connectTo("127.0.0.1", 57621)){
        std::cout<<"Connected to port 57621"<<std::endl;
        udpSocket.disconnect();
        std::cout<<"Disconnected"<<std::endl;
    }else{
        std::cout<<"Failed to connect"<<std::endl;
    }

    return 0;
}