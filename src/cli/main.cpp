#include "../helper.h"

int main(int argc, char* argv[]){

    Scan scan = Scan(Helper::argvToString(argc,argv));
    scan.findIpsAndPortsScanned();
    scan.rawResults = Helper::portscan(argc, argv);

    for(const auto& [key, value] : scan.rawResults){
        if(key == "Help"){
            Helper::printHelpMessage();
            return 0;
        }

        if(key == "Error"){
            std::cout<<value<<std::endl;
            return 1;
        }
    }

    Helper::printResults(scan);

    return 0;
}
