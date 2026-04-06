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
        return "[-] No banner received";

    if(response.rfind("SSH-", 0) == 0){
        size_t end = response.find('\n');
        std::string versionInfo = response.substr(0, end == std::string::npos ? response.size() : end);
        versionInfo.erase(versionInfo.find_last_not_of("\r\n") + 1);

        std::string implementation = "Unknown SSH implementation";

        size_t dashPos = versionInfo.find('-', 4);
        if(dashPos != std::string::npos){
            implementation = versionInfo.substr(dashPos + 1);
        }
        versionInfo = versionInfo.substr(0, dashPos);

        return "[+] SSH detected | Version: " + versionInfo + " | Implementation: " + implementation;
    }

    // HTTP
    if(response.rfind("HTTP/", 0) == 0){
        std::string firstLine = response.substr(0, response.find("\n"));
        return "[+] HTTP detected | Status line: " + firstLine +
               " | " + psSocket::analyseHTTP(response);
    }

    if(response.rfind("220", 0) == 0){
        std::string firstLine = response.substr(0, response.find("\n"));

        if(response.find("SMTP") != std::string::npos ||
           response.find("ESMTP") != std::string::npos){
            return "[+] SMTP detected | Banner: " + firstLine;
        }

        if(response.find("FTP") != std::string::npos){
            return "[+] FTP detected | Banner: " + firstLine;
        }

        return "[?] 220 response detected | Possible FTP/SMTP | Banner: " + firstLine;
    }

    return "[-] Unknown protocol | Raw banner: " + response.substr(0, response.find("\n"));
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
        uint8_t len = static_cast<uint8_t>(end - start);
        packet.push_back(len);
        for(size_t i = start; i < end; i++)
            packet.push_back(domain[i]);

        start = end + 1;
    }

    // last part
    uint8_t len = static_cast<uint8_t>(domain.size() - start);
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

std::vector<uint8_t> psSocket::buildTLSClientHello() {
    std::vector<uint8_t> clientHello;

    // --- TLS Record Header ---
    clientHello.push_back(0x16); // Handshake
    clientHello.push_back(0x03); // TLS major
    clientHello.push_back(0x01); // TLS minor
    clientHello.push_back(0x00); // Length high byte (fill later)
    clientHello.push_back(0x2f); // Length low byte (47 bytes = 4 + 43)

    // --- Handshake Header ---
    clientHello.push_back(0x01); // ClientHello
    clientHello.push_back(0x00);
    clientHello.push_back(0x00);
    clientHello.push_back(0x2b); // 43 bytes body

    // --- ClientHello Body ---
    clientHello.push_back(0x03); // TLS version 1.2
    clientHello.push_back(0x03);

    // Random 32 bytes
    for(int i = 0; i < 32; ++i) clientHello.push_back(0x00);

    // Session ID length
    clientHello.push_back(0x00);

    // Cipher suites length (2 bytes) + cipher suite
    clientHello.push_back(0x00);
    clientHello.push_back(0x02);
    clientHello.push_back(0x00);
    clientHello.push_back(0x2f);

    // Compression methods length + method
    clientHello.push_back(0x01);
    clientHello.push_back(0x00);

    // Extensions length = 0
    clientHello.push_back(0x00);
    clientHello.push_back(0x00);

    return clientHello;
}

std::string psSocket::analyseHTTPS(std::string response) {
    std::string result;

    // TLS response first byte = 0x16 (Handshake) or 0x15 (Alert)
    unsigned char b0 = response[0];
    unsigned char b1 = response[1];

    if((b0 == 0x16 || b0 == 0x15) && b1 == 0x03) {
        result += "[+] HTTPS Detected (TLS handshake/alert response)\n";
    } else {
        result += "[-] Not HTTPS (unexpected response)\n";
        return result; // stop early if not HTTPS
    }

    // Optionally: show raw TLS version detected from server
    // TLS version is bytes 1-2 (major, minor)
    std::string tlsVersion;
    switch(b1) {
        case 0x00: tlsVersion = "SSL 3.0"; break;
        case 0x01: tlsVersion = "TLS 1.0"; break;
        case 0x02: tlsVersion = "TLS 1.1"; break;
        case 0x03: tlsVersion = "TLS 1.2"; break;
        case 0x04: tlsVersion = "TLS 1.3"; break;
        default: tlsVersion = "Unknown TLS version"; break;
    }

    result += "[*] TLS version (from handshake): " + tlsVersion + "\n";

    // Optionally: dump first few bytes of the handshake for debugging
    size_t dumpLen = std::min((size_t)16, response.size());
    result += "[*] Response bytes (first " + std::to_string(dumpLen) + "): ";
    for(size_t i = 0; i < dumpLen; ++i) {
        char buf[4];
        snprintf(buf, sizeof(buf), "%02X ", (unsigned char)response[i]);
        result += buf;
    }
    result += "\n";

    return result;
}