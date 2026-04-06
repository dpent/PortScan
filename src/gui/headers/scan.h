#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class Scan{
    public:
    std::string command;
    std::string outputString;
    std::vector<std::string> ipsScanned;
    std::vector<int> portsScanned;
    std::unordered_map<std::string, std::string> rawResults;

    Scan(std::string command);
    void formatOutputString();
};