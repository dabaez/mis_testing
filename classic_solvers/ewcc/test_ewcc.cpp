#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "ewcc_timelimit.h"
#include <utility>
#include <iomanip>

int main(int argc, char* argv[]) {

    if (argc != 3){
        std::cout<<"Usage: "<<argv[0]<<" filename time_limit"<<std::endl;
        return 1;
    }

    std::ifstream graphFile(argv[1]);

    int nodes=-1,edges;
    std::string line;
    std::vector< std::vector<int> > gr;
    while (getline(graphFile,line)){
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        if (nodes == -1){
            ss>>nodes>>edges;
            gr.resize(nodes);
        } else {
            int u,v;
            ss>>u>>v;
            u--; v--;
            gr[u].push_back(v);
            gr[v].push_back(u);
        }
    }

    graphFile.close();

    std::pair< std::vector<int> , double > ans = ewcc(gr,std::stod(argv[2]));
    std::cout<<nodes-ans.first.size()<<",";
    std::cout<<std::fixed<<std::setprecision(5)<<ans.second<<'\n';

}