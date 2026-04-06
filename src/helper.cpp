#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>
#include "sockets/headers/tcpSocket.h"
#include "servers/headers/tcpServerSocket.h"
#include "sockets/headers/udpSocket.h"
#include "servers/headers/udpServerSocket.h"


struct Args {
    std::vector<char> letters;                  // single-letter flags
    std::unordered_map<std::string, std::string> words; // --word arguments
};

std::vector<int> parseOctet(const std::string& part) {
    std::vector<int> result;

    if (part == "*") {
        for (int i = 0; i <= 255; ++i)
            result.push_back(i);
    }
    else {
        size_t dashPos = part.find('-');
        if (dashPos != std::string::npos) {
            int start = std::stoi(part.substr(0, dashPos));
            int end   = std::stoi(part.substr(dashPos + 1));
            for (int i = start; i <= end; ++i)
                result.push_back(i);
        } else {
            result.push_back(std::stoi(part));
        }
    }

    return result;
}

// Recursive function to generate all combinations
void generateIPs(const std::vector<std::vector<int>>& octets, std::vector<std::string>& result, std::string current = "", size_t index = 0) {
    if (index == octets.size()) {
        result.push_back(current.substr(1)); // remove leading '.'
        return;
    }

    for (int val : octets[index]) {
        generateIPs(octets, result, current + "." + std::to_string(val), index + 1);
    }
}

std::vector<std::string> expandIP(const std::string& ipPattern) {
    std::vector<std::string> result;
    std::vector<std::vector<int>> octets;
    std::stringstream ss(ipPattern);
    std::string part;

    while (std::getline(ss, part, '.')) {
        octets.push_back(parseOctet(part));
    }

    if (octets.size() != 4) {
        std::cerr << "Invalid IP format: " << ipPattern << std::endl;
        return result;
    }

    generateIPs(octets, result);
    return result;
}


Args parseArgs(int argc, char* argv[]){

    Args result;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg.rfind("--", 0) == 0 && arg.length() > 2) {
            // --word argument
            std::string key = arg.substr(2);
            std::string value = "";
            if (i + 1 < argc && argv[i+1][0] != '-') {
                value = argv[i+1];
                ++i; // skip value in next iteration
            }
            result.words[key] = value;
        }
        else if (arg.rfind("-", 0) == 0 && arg.length() > 1) {
            // -letters, can be combined like -abc
            for (size_t j = 1; j < arg.size(); ++j) {
                result.letters.push_back(arg[j]);
            }
        }
    }

    return result;
}

void printResults(const std::unordered_map<std::string, std::string>& resultsPerProbe) {
    for (const auto& kv : resultsPerProbe) {
        std::cout << kv.first << " - " << kv.second << std::endl;
    }
}

void printHelpMessage(){
    std::cout<<"Usage: portscan [options]\n"
    "Options:\n"
        "-h                    Show this help message\n"
        "-v                    Verbose output (show all results)\n"
        "-b                    Scan both TCP and UDP (default is TCP only)\n"
        "-u                    Scan UDP instead of TCP\n"
        "--ip <IP_PATTERN>     IP address or pattern to scan (e.g. 192.168.1.*, 127.0-10.0-24.*)\n"
        "--ports <PORTS>       Comma-separated list of ports/ranges (e.g. 80,443,1000-2000). Default is 0-1024\n"
    <<std::endl;
}

std::unordered_map<std::string, std::string> portscan(int argc, char* argv[]){
    Args args = parseArgs(argc, argv);

    auto helpIt = std::find(args.letters.begin(), args.letters.end(), 'h');
    if(helpIt != args.letters.end()){
        std::unordered_map<std::string, std::string> result;
        result["Help"] = "True";
        return result;
    }

    std::string ip = args.words.count("ip") ? args.words["ip"] : "";
    if(ip.empty()){
        std::unordered_map<std::string, std::string> result;
        result["Error"] = "No IP provided. Use --ip <IPV4_ADDRESS>";
        return result;
    }
    std::vector<std::string> ipsToScan = expandIP(ip);

    std::vector<int> portsToScan;
    if(args.words.count("ports")){
        std::string portsStr = args.words["ports"];
        std::stringstream ss(portsStr);
        std::string portPart;
        while (std::getline(ss, portPart, ',')) {
            std::vector<int> parsedPorts = parseOctet(portPart);
            portsToScan.insert(portsToScan.end(), parsedPorts.begin(), parsedPorts.end());
        }
    }else{
        for(int i = 0; i<=1024; i++){
            portsToScan.push_back(i);
        }
    }
    
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cout << "WSAStartup failed" << std::endl;
        return std::unordered_map<std::string, std::string>{{"Error", "WSAStartup failed"}};
    }
    else {
        std::cout<<"WSAStartup successful" << std::endl;
    }
#endif

    std::unordered_map<std::string, std::string> resultsPerProbe;

    psSocket* socket;
    auto verboseIt = std::find(args.letters.begin(), args.letters.end(), 'v');
    auto allIt = std::find(args.letters.begin(), args.letters.end(), 'b');
    if(allIt != args.letters.end()){
        socket = new TCPSocket();
        
        for(const std::string& ip : ipsToScan){
            for(int& port : portsToScan){
                std::string result = socket->scanPort(ip.c_str(), port);
                if(result == "Nothing on this port" || (verboseIt == args.letters.end() && result == "[-] Connection failed")){
                    continue;
                }else{
                    resultsPerProbe[ip + ":" + std::to_string(port)] = result;
                }
            }
        }

        delete socket;

        socket = new UDPSocket();
        
        for(const std::string& ip : ipsToScan){
            for(int& port : portsToScan){

                if(resultsPerProbe.count(ip + ":" + std::to_string(port))){
                    continue; // already have a result from TCP scan
                }

                std::string result = socket->scanPort(ip.c_str(), port);
                if(result == "Nothing on this port" || (verboseIt == args.letters.end() && result == "[-] No response (open|filtered|closed)")){
                    continue;
                }else{
                    resultsPerProbe[ip + ":" + std::to_string(port)] = result;
                }
            }
        }
        delete socket;
        return resultsPerProbe;
    }

    auto it = std::find(args.letters.begin(), args.letters.end(), 'u');

    if(it != args.letters.end()){
        socket = new UDPSocket();
    }else{
        socket = new TCPSocket();
    }

    for(const std::string& ip : ipsToScan){

        for(int& port : portsToScan){
            std::string result = socket->scanPort(ip.c_str(), port);
            if(result == "Nothing on this port" || (verboseIt == args.letters.end() && result == "[-] Connection failed")
               || (verboseIt == args.letters.end() && result == "[-] No response (open|filtered|closed)")){
                continue;
            }else{
                resultsPerProbe[ip + ":" + std::to_string(port)] = result;
            }
        }
    }

    delete socket;

    return resultsPerProbe;
}