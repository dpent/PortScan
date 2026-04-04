#include "../headers/psServerSocket.h"

psServerSocket::psServerSocket(int port){
    this->port = port;
}

bool psServerSocket::start(){
    return true;
}

void psServerSocket::stop(){
    return;
}