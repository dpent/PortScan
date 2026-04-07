#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include "sockets/headers/tcpSocket.h"
#include "servers/headers/tcpServerSocket.h"
#include "sockets/headers/udpSocket.h"
#include "servers/headers/udpServerSocket.h"
#include "gui/headers/scan.h"
#include <regex>

struct Args {
    std::vector<char> letters;                  // single-letter flags
    std::unordered_map<std::string, std::string> words; // --word arguments

    std::string toString() const {
        std::stringstream ss;
        ss << "Letters: ";
        for (char c : letters) {
            ss << "-" << c << " ";
        }
        ss << "\nWords:\n";
        for (const auto& [key, value] : words) {
            ss << "--" << key << " " << value << "\n";
        }
        return ss.str();
    }
};

class Helper{
    public:
    static std::vector<int> parseOctet(const std::string& part);
    static void generateIPs(const std::vector<std::vector<int>>& octets, std::vector<std::string>& result, std::string current = "", size_t index = 0);
    static std::vector<std::string> expandIP(const std::string& ipPattern);
    static Args parseArgs(int argc, char* argv[]);
    static void printResults(const std::unordered_map<std::string, std::string>& resultsPerProbe);
    static void printHelpMessage();
    static std::string getExtension(std::string& filename);
    static bool exportResults(Scan* scan, std::string& filename);
    static std::vector<std::string> splitByIP(std::string& input);
    static std::string formatMD(std::string& output);
    static std::string formatHTML(std::string& output);
    static std::unordered_map<std::string, std::string> portscan(int argc, char* argv[]);
};