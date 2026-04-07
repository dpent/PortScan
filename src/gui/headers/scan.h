#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <sstream>
#include <iostream>

class Scan{
    public:
    static long id;
    static int numActiveScans;
    static std::mutex activeScansMutex;

    std::string command;
    std::string publicName;
    std::string outputString;
    std::vector<std::string> ipsScanned;
    std::string ipRegex;
    std::vector<int> portsScanned;
    std::string portRegex;
    std::unordered_map<std::string, std::string> rawResults;

    Scan(std::string command);
    void formatOutputString();
    void findIpsAndPortsScanned();
};