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

	return "ScanJob. No attributes.";
}

std::string ScanJob::humanReadableName() {

	return "ScanJob";
}