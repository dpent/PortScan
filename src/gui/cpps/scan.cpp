#include "../headers/scan.h"

void Scan::formatOutputString(){

    for(const auto& [key, value] : rawResults){
        if(key == "Help"){
            outputString = key + " - " + "Usage: portscan [options]\n"
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
        outputString += kv.first + " - " + kv.second + "\n";
    }
}

Scan::Scan(std::string command){
    this->command = command;
    outputString = "";
}