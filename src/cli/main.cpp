#include "../helper.h"

int main(int argc, char* argv[]){

    std::unordered_map<std::string, std::string> results = Helper::portscan(argc, argv);

    for(const auto& [key, value] : results){
        if(key == "Help"){
            Helper::printHelpMessage();
            return 0;
        }

        if(key == "Error"){
            std::cout<<value<<std::endl;
            return 1;
        }
    }

    Helper::printResults(results);

    return 0;
}
