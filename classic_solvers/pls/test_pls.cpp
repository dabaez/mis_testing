#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "pls_timelimit.h"
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
    std::vector< std::vector<bool> > mat;
    std::vector< std::vector<int> > gr;
    while (getline(graphFile,line)){
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        if (nodes == -1){
            ss>>nodes>>edges;
            mat.assign(nodes,std::vector<bool>(nodes,1));
            gr.resize(nodes);
        } else {
            int u,v;
            ss>>u>>v;
            u--; v--;
            mat[u][v] = 0;
            mat[v][u] = 0;
        }
    }

    graphFile.close();

    for (int i=0;i<nodes;i++){
        for (int j=0;j<nodes;j++){
            if (mat[i][j] && i != j){
                gr[i].push_back(j);
            }
        }
    }

    std::pair< std::vector<int> , double > ans = pls(gr,std::stod(argv[2]));
    std::cout<<ans.first.size()<<",";
    std::cout<<std::fixed<<std::setprecision(5)<<ans.second<<'\n';

}