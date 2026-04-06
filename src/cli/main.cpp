#include "../helper.cpp"

int main(int argc, char* argv[]){

    std::unordered_map<std::string, std::string> results = portscan(argc, argv);

    for(const auto& [key, value] : results){
        if(key == "Help"){
            printHelpMessage();
            return 0;
        }

        if(key == "Error"){
            std::cout<<value<<std::endl;
            return 1;
        }
    }

    printResults(results);

    return 0;
}
