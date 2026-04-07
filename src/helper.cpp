#include "helper.h"

std::vector<int> Helper::parseOctet(const std::string& part) {
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
void Helper::generateIPs(const std::vector<std::vector<int>>& octets, std::vector<std::string>& result, std::string current, size_t index) {
    if (index == octets.size()) {
        result.push_back(current.substr(1)); // remove leading '.'
        return;
    }

    for (int val : octets[index]) {
        Helper::generateIPs(octets, result, current + "." + std::to_string(val), index + 1);
    }
}

std::vector<std::string> Helper::expandIP(const std::string& ipPattern) {
    std::vector<std::string> result;
    std::vector<std::vector<int>> octets;
    std::stringstream ss(ipPattern);
    std::string part;

    while (std::getline(ss, part, '.')) {
        octets.push_back(Helper::parseOctet(part));
    }

    if (octets.size() != 4) {
        std::cerr << "Invalid IP format: " << ipPattern << std::endl;
        return result;
    }

    generateIPs(octets, result);
    return result;
}


Args Helper::parseArgs(int argc, char* argv[]){

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

void Helper::printResults(Scan& scan) {
    scan.formatOutputString();
    std::cout<<scan.outputString<<std::endl;
}

void Helper::printHelpMessage(){
    std::cout<<"Usage: portscan [options]\n"
    "Options:\n"
        "-h                    Show this help message\n"
        "-v                    Verbose output (show all results)\n"
        "-b                    Scan both TCP and UDP (default is TCP only)\n"
        "-u                    Scan UDP instead of TCP\n"
        "--ip <IP_PATTERN>     IP address or pattern to scan (e.g. 192.168.1.*, 127.0-10.0-24.*)\n"
        "--ports <PORTS>       Comma-separated list of ports/ranges (e.g. 80,443,1000-2000). Default is 0-1024\n"
        "--out <FILEPATH>      Filepath of the file you want the results to be exported to. Only in .html, .md, .txt formats."
    <<std::endl;
}

std::unordered_map<std::string, std::string> Helper::portscan(int argc, char* argv[]){
    
    Args args = Helper::parseArgs(argc, argv);

    //std::cout<<args.toString()<<std::endl;

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
    std::vector<std::string> ipsToScan = Helper::expandIP(ip);

    std::vector<int> portsToScan;
    if(args.words.count("ports")){
        std::string portsStr = args.words["ports"];
        std::stringstream ss(portsStr);
        std::string portPart;
        while (std::getline(ss, portPart, ',')) {
            std::vector<int> parsedPorts = Helper::parseOctet(portPart);
            portsToScan.insert(portsToScan.end(), parsedPorts.begin(), parsedPorts.end());
        }
    }else{
        for(int i = 0; i<=1024; i++){
            portsToScan.push_back(i);
        }
    }

    std::unordered_map<std::string, std::string> resultsPerProbe;

    if(args.words.count("out")){
        std::string outputFilename = args.words["out"];
        
        std::string extention = Helper::getExtension(outputFilename);
        if(extention != ".html" && extention != ".md" && extention != ".txt"){
            std::unordered_map<std::string, std::string> result;
            result["Error"] = "Wrong file extention provided. .html, .md or .txt allowed.";
            return result;
        }

        resultsPerProbe["Saved at"] = outputFilename;
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

std::string Helper::getExtension(std::string& filename){
    return std::filesystem::path(filename).extension().string();
}

bool Helper::exportResults(Scan* scan, std::string& filename){

    if (!scan) return false;

    std::ofstream out(filename);
    if (!out.is_open()) return false;

    std::string ext = Helper::getExtension(filename);
    
    if (ext == ".html") {
        out << "<!DOCTYPE html>\n<html>\n<head>\n";
        out << "<meta charset=\"UTF-8\">\n<title>Scan Results</title>\n</head>\n<body>\n";

        out << "<h2>Command</h2>\n<p>" << scan->command << "</p>\n";
        
        out << "<h3>IP regex</h3>\n<p>" << scan->ipRegex << "</p>\n";

        out << "<h3>Port regex</h3>\n<p>" << scan->portRegex << "</p>\n";

        out << "<h2>Output</h2>\n<p>" << Helper::formatHTML(scan->outputString) << "</p>\n";

        out << "<h3>IPv4s scanned</h3>\n<ul>\n";
        for (const auto& ip : scan->ipsScanned) {
            out << "<li>" << ip << "</li>\n";
        }
        out << "</ul>\n";

        out << "\n<h3>Ports scanned</h3>\n<ul>\n";
        for (const auto& port : scan->portsScanned) {
            out << "<li>" << port << "</li>\n";
        }
        out << "</ul>\n";

        out << "</body>\n</html>";
    }
    else if (ext == ".md") {
        out << "# PortScan results\n"; 

        out << "## Command\n";
        out << scan->command << "\n";
        
        out << "### IP regex\n";
        out << scan->ipRegex << "\n";

        out << "### Port regex\n";
        out << scan->portRegex << "\n";

        out << "## Output\n";
        out << Helper::formatMD(scan->outputString) << "\n\n";

        out << "### IPv4s scanned\n";
        for (const auto& ip : scan->ipsScanned) {
            out << "- " << ip << "\n";
        }

        out << "### Ports scanned\n";
        for (const auto& port : scan->portsScanned) {
            out << "- " << port << "\n";
        }
    }
    else {
        out << "Output:\n";
        out << scan->outputString << "\n\n";

        out << "Command\n";
        out << scan->command << "\n";
        
        out << "IP regex\n";
        out << scan->ipRegex << "\n";

        out << "Port regex\n";
        out << scan->portRegex << "\n";

        out << "IPv4s scanned:\n";
        for (const auto& ip : scan->ipsScanned) {
            out << "- " << ip << "\n";
        }

        out << "\nPorts scanned\n";
        for (const auto& port : scan->portsScanned) {
            out << "- " << port << "\n";
        }
    }

    out.close();

    return true;
}

std::vector<std::string> Helper::splitByIP(std::string& input) {
    std::vector<std::string> entries;

    // Matches IP:port at the start of an entry
    std::regex ipPortRegex(R"((\d{1,3}\.){3}\d{1,3}:\d+)");

    std::sregex_iterator iter(input.begin(), input.end(), ipPortRegex);
    std::sregex_iterator end;

    size_t lastPos = 0;
    for (; iter != end; ++iter) {
        size_t matchPos = iter->position();
        if (matchPos != lastPos) {
            // Add everything between lastPos and this match as one entry
            std::string entry = input.substr(lastPos, matchPos - lastPos);
            if (!entry.empty() && entry.find_first_not_of(" \n\r\t") != std::string::npos)
                entries.push_back(entry);
        }
        lastPos = matchPos;
    }

    // Add the last entry from lastPos to the end
    std::string lastEntry = input.substr(lastPos);
    if (!lastEntry.empty() && lastEntry.find_first_not_of(" \n\r\t") != std::string::npos)
        entries.push_back(lastEntry);

    return entries;
}

std::string Helper::formatMD(std::string& output) {
    std::ostringstream oss;
    auto entries = Helper::splitByIP(output);

    for (const auto& e : entries) {
        oss << "- " << e << "\n";
    }
    return oss.str();
}

std::string Helper::formatHTML(std::string& output) {
    std::ostringstream oss;
    auto entries = Helper::splitByIP(output);

    oss << "<ul>\n";
    for (const auto& e : entries) {
        oss << "  <li><code>" << e << "</code></li>\n";
    }
    oss << "</ul>\n";
    return oss.str();
}

std::string Helper::argvToString(int argc, char* argv[]) {
    std::ostringstream oss;
    for (int i = 0; i < argc; ++i) {
        if (i > 0) oss << " ";       // add space between arguments
        oss << argv[i];
    }
    return oss.str();
}