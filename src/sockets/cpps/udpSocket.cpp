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

    DWORD timeout = 2000; // 2 seconds in ms
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    inet_pton(AF_INET, ip, &(addr.sin_addr));
    #else

    timeval tv;
    tv.tv_sec = 2;   // 2 seconds
    tv.tv_usec = 0;

    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

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

Response UDPSocket::receiveBytes(){
    char* buffer = new char[1024];
    socklen_t addrLen = sizeof(addr);
    
    #ifdef _WIN32
    int bytesReceived = recvfrom(s, buffer, 1024, 0, (sockaddr*)&addr, &addrLen);
    #else   
    ssize_t bytesReceived = recvfrom(s, buffer, 1024, 0, (sockaddr*)&addr, &addrLen);
    #endif
    if(bytesReceived < 0){
        delete[] buffer;
        return {nullptr, 0};
    }

    return {buffer, bytesReceived};
}

std::string UDPSocket::scanPort(const char* ip, int port){

    std::string scanResult = "Nothing on this port";

    if(connectTo(ip, port)){

        scanResult = "[-] No response (open|filtered|closed)";

        //Send mock NTP query
        std::vector<uint8_t> query = psSocket::buildNTPQuery();
        sendBytes((char*)query.data(), query.size());
    
        Response response = receiveBytes();
        if(response.data && response.length >= 48){
    
            scanResult = "[+] NTP Detected. Got a response to a mock NTP query.";
            delete[] response.data;
            return scanResult;
        }
    
        disconnect();

    }else{

    }

    if(connectTo(ip, port)){

        scanResult = "[-] No response (open|filtered)";

        //Send mock DNS query
        std::vector<uint8_t> query = psSocket::buildDNSQuery("google.com");
        sendBytes((char*)query.data(), query.size());
    
        Response response = receiveBytes();
        if(response.data && response.length >= 48){
    
            scanResult = "[+] DNS Detected. Got a response to a mock DNS query.";
            delete[] response.data;
            return scanResult;
        }
    
        disconnect();

    }else{

    }

    if(connectTo(ip, port)){

        scanResult = "[-] No response (open|filtered)";
        //std::cout<<"Connected to port "<<port<<std::endl;
        
        //Send mock SNMP query
        std::vector<uint8_t> query = psSocket::buildSNMPQuery();
        sendBytes((char*)query.data(), query.size());
        //std::cout<<"Sent data"<<std::endl;

        Response response = receiveBytes();
        if(response.data && response.length > 10){

            scanResult = "[+] SNMP Detected. Got a response to a mock SNMP query.";
            delete[] response.data;
            return scanResult;
        }

        disconnect();

        //std::cout<<"Disconnected"<<std::endl;
    }else{
        //std::cout<<"Failed to connect"<<std::endl;
    }

    return scanResult;
}