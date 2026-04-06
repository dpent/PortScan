#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

class Scan{
    public:
    static ulong id;
    static uint numActiveScans;
    static std::mutex activeScansMutex;

    std::string command;
    std::string publicName;
    std::string outputString;
    std::vector<std::string> ipsScanned;
    std::vector<int> portsScanned;
    std::unordered_map<std::string, std::string> rawResults;

    Scan(std::string command);
    void formatOutputString();
};