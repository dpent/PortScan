#include "../headers/job.h"
#include <sstream>
#include "../../helper.h"
#include <cstring>

Job::Job() {
}

Job::~Job() {
}

void Job::execute() {
	std::cout << "This is a base job. It only prints this to the console." << std::endl;
}

std::string Job::toString() {

	return "Base Job. No attributes.";
}

std::string Job::humanReadableName() {
	
	return "Job";
}


PrepareForJoinJob::PrepareForJoinJob() {
}

PrepareForJoinJob::~PrepareForJoinJob() {
}

void PrepareForJoinJob::execute() {
}

std::string PrepareForJoinJob::toString() {

	return "PrepareForJoinJob. No attributes.";
}

std::string PrepareForJoinJob::humanReadableName() {

	return "PrepareForJoinJob";
}

ScanJob::ScanJob(std::string command){
    this->command = command;
}

ScanJob::~ScanJob(){
}

void ScanJob::execute(){

    Scan* newScan = new Scan(command);
    newScan->outputString = "Command is ("+command+"). Scanning, please wait for results...";
    Engine::historyMutex.lock();
    Engine::history.push_back(newScan);
    Engine::historyMutex.unlock();

    std::istringstream iss(command);
    std::vector<std::string> tokens;
    std::string token;
    tokens.push_back("portscan");
    while (iss >> token) {
        tokens.push_back(token);
    }

    // Prepare argc and argv
    int argc = static_cast<int>(tokens.size());
    std::vector<char*> argv;
    for (auto& s : tokens) {
        argv.push_back(const_cast<char*>(s.c_str()));
    }
    
    newScan->findIpsAndPortsScanned();
    newScan->rawResults = Helper::portscan(argc, argv.data());
    newScan->formatOutputString();
    newScan->publicName = newScan->command;

    Scan::activeScansMutex.lock();
    Scan::numActiveScans--;
    Scan::activeScansMutex.unlock();

}

std::string ScanJob::toString() {

	return "ScanJob. command variable is: "+command;
}

std::string ScanJob::humanReadableName() {

	return "ScanJob";
}

ExportJob::ExportJob(std::string filepath, uint8_t type, Scan* scan){
    this->filepath = filepath;
    this->type = type;
    this->scan = scan;
}

ExportJob::~ExportJob(){
}

void ExportJob::execute(){

    if (!scan) return;

    std::ofstream out(filepath);
    if (!out.is_open()) return;
    
    if (type == 0) {
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
    else if (type == 1) {
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

    return;
}

std::string ExportJob::toString() {

	return "ExportJob. filepath variable is: "+filepath;
}

std::string ExportJob::humanReadableName() {

	return "ExportJob";
}