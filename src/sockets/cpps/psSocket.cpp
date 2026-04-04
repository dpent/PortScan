#include "../headers/psSocket.h"

psSocket::psSocket(){
    return;
}

bool psSocket::connectTo(const char* ip, int port){

    return true;
}

void psSocket::disconnect(){
    return;
}

bool psSocket::sendBytes(char* buffer, int length){
    return true;
}

Response psSocket::receiveBytes(){
    return {nullptr, 0};
}

std::string psSocket::scanPort(const char* ip, int port){
    return "Stuff";
}

std::string psSocket::analyseHTTP(std::string response){

    std::string result;

    if(response.find("HTTP/") == 0){
        result += "[+] HTTP Detected\n";
    } else {
        result += "[-] Not HTTP but responded to HTTP request\n";
        return result; // stop early if not HTTP
    }

    size_t pos = response.find("\r\n");
    if(pos != std::string::npos){
        std::string statusLine = response.substr(0, pos);
        result += "[*] Status: " + statusLine + "\n";
    }

    size_t headersEnd = response.find("\r\n\r\n");
    if(headersEnd != std::string::npos){
        std::string headers = response.substr(0, headersEnd);

        size_t start = 0;
        while(true){
            size_t end = headers.find("\r\n", start);
            if(end == std::string::npos) break;

            std::string line = headers.substr(start, end - start);
            start = end + 2;

            result += "[H] " + line + "\n";
        }
    }

    auto extractHeader = [&](const std::string& name){ //Specific headers
        size_t p = response.find(name);
        if(p != std::string::npos){
            size_t end = response.find("\r\n", p);
            return response.substr(p, end - p);
        }
        return std::string("");
    };

    std::string server = extractHeader("Server:");
    if(!server.empty()){
        result += "[+] " + server + "\n";
    }

    std::string contentType = extractHeader("Content-Type:");
    if(!contentType.empty()){
        result += "[+] " + contentType + "\n";
    }

    return result;
}

std::string psSocket::analyseBanner(std::string response){
    if(response.empty())
        return "[-] No banner";

    if(response.find("SSH-") == 0){
        return "[+] SSH";
    }

    if(response.find("HTTP/") == 0){
        return psSocket::analyseHTTP(response);
    }

    if(response.find("220") == 0){

        if(response.find("SMTP") != std::string::npos ||
           response.find("ESMTP") != std::string::npos){
            return "[+] SMTP";
        }

        if(response.find("FTP") != std::string::npos){
            return "[+] FTP";
        }

        return "[?] 220 (FTP/SMTP unknown)";
    }

    return "[-] Unknown protocol";
}

std::vector<uint8_t> psSocket::buildDNSQuery(std::string domain){
    std::vector<uint8_t> packet;

    // --- Header (12 bytes) ---
    uint16_t id = htons(0x1234);       // random ID
    uint16_t flags = htons(0x0100);    // standard query
    uint16_t qdcount = htons(1);       // 1 question
    uint16_t ancount = 0;
    uint16_t nscount = 0;
    uint16_t arcount = 0;

    packet.insert(packet.end(), (uint8_t*)&id, (uint8_t*)&id + 2);
    packet.insert(packet.end(), (uint8_t*)&flags, (uint8_t*)&flags + 2);
    packet.insert(packet.end(), (uint8_t*)&qdcount, (uint8_t*)&qdcount + 2);
    packet.insert(packet.end(), (uint8_t*)&ancount, (uint8_t*)&ancount + 2);
    packet.insert(packet.end(), (uint8_t*)&nscount, (uint8_t*)&nscount + 2);
    packet.insert(packet.end(), (uint8_t*)&arcount, (uint8_t*)&arcount + 2);

    // --- Question section ---

    // Convert domain to DNS format: example.com → 7example3com0
    size_t start = 0;
    size_t end;

    while((end = domain.find('.', start)) != std::string::npos){
        uint8_t len = end - start;
        packet.push_back(len);
        for(size_t i = start; i < end; i++)
            packet.push_back(domain[i]);

        start = end + 1;
    }

    // last part
    uint8_t len = domain.size() - start;
    packet.push_back(len);
    for(size_t i = start; i < domain.size(); i++)
        packet.push_back(domain[i]);

    packet.push_back(0x00); // end of name

    // QTYPE = A (1)
    uint16_t qtype = htons(1);
    packet.insert(packet.end(), (uint8_t*)&qtype, (uint8_t*)&qtype + 2);

    // QCLASS = IN (1)
    uint16_t qclass = htons(1);
    packet.insert(packet.end(), (uint8_t*)&qclass, (uint8_t*)&qclass + 2);

    return packet;
}

std::vector<uint8_t> psSocket::buildNTPQuery(){
    std::vector<uint8_t> packet(48, 0); // 48 bytes initialized to 0

    // LI = 0, VN = 4, Mode = 3 (client)
    packet[0] = 0x23;

    return packet;
}

std::vector<uint8_t> psSocket::buildSNMPQuery() {
    // Very minimal SNMPv1 GET request for sysDescr
    std::vector<uint8_t> packet = {
        0x30, 0x26,             // Sequence, length 38
        0x02, 0x01, 0x00,       // Version = 0 (v1)
        0x04, 0x06,             // Community string length = 6
        'p','u','b','l','i','c',// Community = "public"
        0xa0, 0x19,             // GetRequest PDU, length 25
        0x02, 0x04, 0x00,0x00,0x00,0x01, // Request ID
        0x02, 0x01, 0x00,       // Error status
        0x02, 0x01, 0x00,       // Error index
        0x30, 0x0b,             // Varbind sequence, length 11
        0x30, 0x09,             // Varbind
        0x06, 0x05, 0x2b,0x06,0x01,0x02,0x01, // OID 1.3.6.1.2.1
        0x05, 0x00              // Null value
    };
    return packet;
}