#include "../headers/tcpSocket.h"

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

Response TCPSocket::receiveBytes(){
    char* buffer = new char[1024];
    #ifdef _WIN32
    int bytesReceived = recv(s, buffer, 1024, 0);
    #else
    ssize_t bytesReceived = recv(s, buffer, 1024, 0);
    #endif
    if(bytesReceived < 0){
        delete[] buffer;
        return {nullptr, 0};
    }

    return {buffer, bytesReceived};
}

std::string TCPSocket::scanPort(const char* ip, int port){

    std::string scanResult = "Nothing on this port";

    if(connectTo(ip, port)){
        //Some protocols like SSH, FTP or SMTP will send a banner on connection
        Response banner = receiveBytes();
        if(banner.data){
            scanResult = psSocket::analyseBanner(std::string(banner.data));
            if (scanResult != "")
            {
                
                delete[] banner.data;
                return scanResult;
            }

            delete[] banner.data;
            
        }

        //std::cout<<"Connected to port "<<port<<std::endl;

        std::vector<uint8_t> tlsClientHello = psSocket::buildTLSClientHello();
        sendBytes((char*)tlsClientHello.data(), static_cast<int>(tlsClientHello.size()));
        std::string request = "GET / HTTP/1.1\r\nHost: " + std::string(ip) + "\r\n\r\n";
        sendBytes((char*)request.c_str(), static_cast<int>(request.length()));
        std::vector<uint8_t> telnetPacket = psSocket::buildTelnetProbe();
        sendBytes((char*)telnetPacket.data(), static_cast<int>(telnetPacket.size()));

        Response response = receiveBytes();
        if(response.data){
            scanResult = psSocket::analyseHTTPS(std::string(response.data, response.length));
            if(scanResult != "[-] Not HTTPS (unexpected response)\n"){
                delete[] response.data;
                return scanResult;
            }   

            scanResult = psSocket::analyseHTTP(std::string(response.data));
            if(scanResult != "[-] Not HTTP but responded to HTTP request\n"){
                delete[] response.data;
                return scanResult;
            }

            if(psSocket::isValidTelnet(response)){
                delete[] response.data;
                return "[+] Telnet detected. Got an answer to a probe.";
            }

            delete[] response.data;

        }

        disconnect();
    }else{
        scanResult = "[-] Connection failed";
    }

    return scanResult;
}