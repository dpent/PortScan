#include "../headers/scan.h"
#include "../../helper.h"

ulong Scan::id = 0;
uint Scan::numActiveScans = 0;
std::mutex Scan::activeScansMutex;

void Scan::formatOutputString(){

    outputString = "";

    if(rawResults.empty()){
        outputString += "Nothing found on this scan.";
        return;
    }

    for(const auto& [key, value] : rawResults){
        if(key == "Help"){
            outputString += key + " - " + "Usage: portscan [options]\n"
            "Options:\n"
            "-h                    Show this help message\n"
            "-v                    Verbose output (show all results)\n"
            "-b                    Scan both TCP and UDP (default is TCP only)\n"
            "-u                    Scan UDP instead of TCP\n"
            "--ip <IP_PATTERN>     IP address or pattern to scan (e.g. 192.168.1.*, 127.0-10.0-24.*)\n"
            "--ports <PORTS>       Comma-separated list of ports/ranges (e.g. 80,443,1000-2000). Default is 0-1024\n";
            return;
        }

    }

    for (const auto& kv : rawResults) {
        if(kv.first == "Saved at"){
            continue;
        }
        outputString += kv.first + " - " + kv.second + "\n";
    }

    if(rawResults.count("Saved at")){

        if(!Helper::exportResults(this, rawResults["Saved at"])){
            outputString += "###Couldn't save results in file: " + rawResults["Saved at"] + "###\n";
        }else{
            outputString += "###Saved at: " + rawResults["Saved at"] + "###\n";
        }
    }
}

void Scan::findIpsAndPortsScanned(){

    std::istringstream iss(command);
    std::vector<std::string> tokens;
    std::string token;
    tokens.push_back("portscan");
    while (iss >> token) {
        tokens.push_back(token);
    }

    for (int i = 1; i < tokens.size(); ++i) {
        std::string arg = tokens[i];

        if (arg.rfind("--", 0) == 0 && arg.length() > 2) {
            // --word argument
            std::string key = arg.substr(2);
            std::string value = "";
            if (i + 1 < tokens.size() && tokens[i+1][0] != '-') {
                value = tokens[i+1];
                ++i; // skip value in next iteration
            }

            if(key == "ip"){
                ipsScanned = Helper::expandIP(value);
                ipRegex = value;
            }

            if(key == "ports"){
                
                std::stringstream ss(value);
                std::string portPart;
                while (std::getline(ss, portPart, ',')) {
                    std::vector<int> parsedPorts = Helper::parseOctet(portPart);
                    portsScanned.insert(portsScanned.end(), parsedPorts.begin(), parsedPorts.end());
                }
                portRegex = portPart;    
            }
        }
    }

    if(portsScanned.size()==0){
        for(int i=0; i<=1024; i++){
            portsScanned.push_back(i);
        }
        portRegex = "0-1024";
    }

}

Scan::Scan(std::string command){
    id++;
    this->command = command;
    outputString = "";
    publicName = "Scanning...";
}