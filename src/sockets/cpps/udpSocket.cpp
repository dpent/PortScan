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
    return true;
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
        sendBytes((char*)query.data(), static_cast<int>(query.size()));
    
        Response response = receiveBytes();
        if(response.data && response.length >= 48){
    
            scanResult = "[+] NTP Detected. Got a response to a mock NTP query.\n";
            delete[] response.data;
            return scanResult;
        }
    
        disconnect();

    }

    if(connectTo(ip, port)){

        scanResult = "[-] No response (open|filtered|closed)";

        //Send mock DNS query
        std::vector<uint8_t> query = psSocket::buildDNSQuery("google.com");
        sendBytes((char*)query.data(), static_cast<int>(query.size()));
        sendBytes((char*)query.data(), static_cast<int>(query.size()));
    
        Response response = receiveBytes();
        if(response.data && response.length >= 48){
    
            scanResult = "[+] DNS Detected. Got a response to a mock DNS query.\n";
            delete[] response.data;
            return scanResult;
        }
    
        disconnect();

    }

    if(connectTo(ip, port)){

        scanResult = "[-] No response (open|filtered|closed)";

        //Send mock TFTP query
        std::vector<uint8_t> query = psSocket::buildTFTPRequest("randomFile.txt");
        sendBytes((char*)query.data(), static_cast<int>(query.size()));

        Response response = receiveBytes();
        if(psSocket::isValidTFTP(response)){
    
            scanResult = "[+] TFTP Detected. Got a response to a mock TFTP query.\n";
            delete[] response.data;
            return scanResult;
        }
    
        disconnect();

    }

    if(connectTo(ip, port)){

        scanResult = "[-] No response (open|filtered|closed)";
        //std::cout<<"Connected to port "<<port<<std::endl;
        
        std::vector<uint8_t> query = psSocket::buildSNMPQuery();
        sendBytes((char*)query.data(), static_cast<int>(query.size()));
        //std::cout<<"Sent data"<<std::endl;

        Response response = receiveBytes();
        if(response.data && response.length > 10){

            if (response.data[0] == 0x30 && response.data[2] == 0x00) {
                scanResult = "[+] SNMP Detected. Got a valid-looking SNMP response.\n";
            } else {
                scanResult = "[-] Response received, but doesn't look like SNMP.\n";
            }
            delete[] response.data;
            return scanResult;
        }

        disconnect();
    }

    return scanResult;
}