#include <vector>
#include <random>
#include <chrono>
#include <queue>
#include <functional>
#include <unordered_set>
#include <iostream>
#include <time.h>
#include <utility>

int map_to_ns(int deg){
    return (deg <3 )?deg :3 ;
}

void change_mapd(int node,int del,std::vector<int> &map_degree,
                 std::vector< std::unordered_set<int> > &NSk,std::vector<int> &active,
                 std::vector<int> &sizeNSk){
    int mapd = map_degree[node];
    int mapdn = mapd + del;
    sizeNSk[map_to_ns(mapd)]--;
    sizeNSk[map_to_ns(mapdn)]++;
    if (map_to_ns(mapd) != map_to_ns(mapdn)){
        if (active[node] == 0 || mapd == 3) NSk[ map_to_ns(mapd) ].erase(node);
        if (active[node] == 0 || mapdn == 3) NSk[ map_to_ns(mapdn) ].insert(node);
    }
    map_degree[node] = mapdn;
}

void swap(int node, int c_iter,
          std::vector< std::vector<int> > &G,
          std::vector<bool> &in_S,std::vector<int> &active, std::vector<int> &sizeNSk,
          std::vector<int> &map_degree, std::vector<int> &exp_degree,std::vector<int> &div_degree,
          std::vector< std::unordered_set<int> > &NSk, std::vector< std::unordered_set<int> > &ne_in_S,
          std::unordered_set<int> &S,
          std::priority_queue< std::pair<int,int> , std::vector< std::pair<int,int> > , std::greater< std::pair<int,int> > > tab_elem,
          std::mt19937 &rng){
            
    int k = map_degree[node];
    //std::cout<<"K "<<k<<std::endl;
    //std::cout<<node<<std::endl;
    NSk[map_to_ns(k)].erase(node);
    sizeNSk[map_to_ns(k)]--;
    S.insert(node);
    in_S[node] = true;
    exp_degree[node] = 0;
    map_degree[node] = -1;
    div_degree[node] = -1;
    ne_in_S[node].clear();

    std::vector<int> vjs;

    /*
    for (int i=6;i<7;i++){
        std::cout<<i<<'\n';
        for (int ne:G[i]) std::cout<<ne<<' ';
        std::cout<<std::endl;
    }
    */

    //std::cout<<"via"<<std::endl;
    for (int via:G[node]){
        //std::cout<<via<<std::endl;
        if (in_S[via]){
            // not adding them to NSk because of tabu!
            sizeNSk[1]++;
            vjs.push_back(via);
            map_degree[via] = 1;
            exp_degree[via] = -1;
            div_degree[via] = G[via].size()-1;
            ne_in_S[via].insert(node);
            S.erase(via);
            in_S[via] = false;
        } else {
            div_degree[via]--;
            change_mapd(via,1,map_degree,NSk,active,sizeNSk);
            if (map_degree[via] == 1) exp_degree[node]++;
            ne_in_S[via].insert(node);
        }
    }

    //std::cout<<"vjs"<<std::endl;
    for (int vj:vjs){
        //std::cout<<"vj "<<vj<<std::endl;
        for (int vja:G[vj]){
            //std::cout<<vja<<std::endl;
            if (vja == node) continue;
            div_degree[vja]++;
            //std::cout<<"erase vj"<<std::endl;
            ne_in_S[vja].erase(vj);
            //std::cout<<"change mapd"<<std::endl;
            change_mapd(vja,-1,map_degree,NSk,active,sizeNSk);
            // if map_degree drops to 0, tabu rules stop applying
            //std::cout<<"REST"<<std::endl;
            if (map_degree[vja] == 0 && active[vja] != 0){
                active[vja] = 0;
                NSk[0].insert(vja);
            }
            if (map_degree[vja] == 1){
                for (const auto& vjp:ne_in_S[vja]){
                    exp_degree[vjp]++;
                }
            }
        }
    }

    //std::cout<<"tabu"<<std::endl;
    for (int vj:vjs){
        //tabu
        if (vjs.size() == 1){
            if (sizeNSk[1] < sizeNSk[2] + sizeNSk[3]){
                std::uniform_int_distribution<> distr(0,sizeNSk[1]-1);
                active[vj] = c_iter + 11 + distr(rng);
            } else {
                active[vj] = c_iter + 1 + sizeNSk[1];
            }
        } else {
            active[vj] = c_iter + 8;
        }
        tab_elem.emplace(active[vj],vj);
    }

    //std::cout<<"end"<<std::endl;

}

void change_ans(std::vector<int> &S_ans, std::unordered_set<int> &S){
    S_ans.clear();
    for (const auto& node:S){
        S_ans.push_back(node);
    }
}

std::pair< std::vector<int> , double > sbts(std::vector< std::vector<int> > &G, double time_limit,int seed = -1){

    clock_t start = clock(), istart;

    std::vector<int> S_ans;
    int graph_size = G.size();

    if (seed == -1) seed = std::chrono::steady_clock::now().time_since_epoch().count();

    std::cout<<"Seed "<<seed<<'\n';
    std::mt19937 rng(seed);
    std::uniform_int_distribution<> flip_coin(0,1);

    std::vector<bool> in_S(graph_size);
    std::vector<int> map_degree(graph_size),exp_degree(graph_size),div_degree(graph_size);
    std::vector<int> active(graph_size), sizeNSk(4);
    // ne_in_S uses quadratic memory
    std::vector< std::unordered_set<int> > NSk(4), ne_in_S(graph_size);
    std::unordered_set<int> S;
    std::priority_queue< std::pair<int,int> , std::vector< std::pair<int,int> > , std::greater< std::pair<int,int> > > tab_elem;

    int iters_max = 100,restart = 0;

    while ( ((double)(clock() - start))/CLOCKS_PER_SEC < time_limit ){

        ++restart;

        //std::cout<<restart<<" BEST "<<S_ans.size()<<std::endl;

        S.clear();
        while (!tab_elem.empty()) tab_elem.pop();
        for (int i=0;i <3 +1;i++){
            NSk[i].clear();
            sizeNSk[i] = 0;
        }
        sizeNSk[0] = graph_size;
        for (int i=0;i<graph_size;i++){
            ne_in_S.clear();
            in_S[i] = false;
            active[i] = 0;
            map_degree[i] = 0;
            exp_degree[i] = -1;
            div_degree[i] = G[i].size();
            NSk[0].insert(i);
        }

        while (NSk[0].size()){
            std::uniform_int_distribution<> distr(0,NSk[0].size()-1);
            int chosen = distr(rng);
            auto itr = NSk[0].begin();
            while (chosen--) itr++;
            int node = *itr;
            swap(node,0,G,in_S,active,sizeNSk,map_degree,exp_degree,div_degree,NSk,ne_in_S,S,tab_elem,rng);

        }

        if (S.size() > S_ans.size()) change_ans(S_ans,S);

        if (restart == 1) istart = clock();

        for (int iters=1;iters<=iters_max && ((double)(clock() - start))/CLOCKS_PER_SEC < time_limit;iters++){

            /*

            std::cout<<S.size()<<std::endl;

            for (const auto& elem:S){
                std::cout<<elem<<' ';
            }
            std::cout<<std::endl;

            std::cout<<"map degree: ";
            for (int i=0;i<graph_size;i++) std::cout<<map_degree[i]<<" ";
            std::cout<<std::endl<<std::endl;

            */

            while ( (!tab_elem.empty()) && tab_elem.top().first <= iters){
                if (active[tab_elem.top().second] == tab_elem.top().first){
                    active[tab_elem.top().second] = 0;
                    if ( map_to_ns(map_degree[tab_elem.top().second]) != 3 ) NSk[ map_to_ns(map_degree[tab_elem.top().second]) ].insert(tab_elem.top().second);
                }
                tab_elem.pop();
            }

            if (!NSk[0].empty()){
                //std::cout<<0<<std::endl;
                std::uniform_int_distribution<> distr(0,NSk[0].size()-1);
                int chosen = distr(rng);
                auto itr = NSk[0].begin();
                while (chosen--) itr++;
                int node = *itr;
                swap(node,0,G,in_S,active,sizeNSk,map_degree,exp_degree,div_degree,NSk,ne_in_S,S,tab_elem,rng);
                if (S.size() > S_ans.size()) change_ans(S_ans,S);
            } else if (!NSk[1].empty()){
                //std::cout<<1<<std::endl;
                std::vector<int> NS1;
                int ed;
                for (const auto& vi:NSk[1]){
                    int ned = exp_degree[*ne_in_S[vi].begin()];
                    if (NS1.empty()){
                        NS1.push_back(vi);
                        ed = ned;
                    } else {
                        if (ned > ed){
                            NS1.clear();
                            ed = ned;
                        }
                        if (ed == ned) NS1.push_back(vi);
                    }
                }
                std::vector<int> NS1dd;
                for (int x:NS1){
                    if (NS1dd.empty()) NS1dd.push_back(x);
                    else {
                        if (div_degree[x] > div_degree[NS1dd[0]]){
                            NS1dd.clear();
                            NS1dd.push_back(x);
                        }
                        if (div_degree[x] == div_degree[NS1dd[0]]){
                            NS1dd.push_back(x);
                        }
                    }
                }
                std::uniform_int_distribution<> distr(0,NS1dd.size()-1);
                int chosen = distr(rng);
                swap(NS1dd[chosen],0,G,in_S,active,sizeNSk,map_degree,exp_degree,div_degree,NSk,ne_in_S,S,tab_elem,rng);
            } else {
                //std::cout<<2<<std::endl;
                if ( sizeNSk[1] > sizeNSk[2] + sizeNSk[3] ){
                    std::vector<int> lardd;
                    for (const auto& vi:NSk[3]){
                        if (active[vi] == 0){
                            if (lardd.empty()) lardd.push_back(vi);
                            else {
                                if (div_degree[ lardd[0] ] == div_degree[vi]){
                                    lardd.push_back(vi);
                                }
                                if (div_degree[ lardd[0] ] < div_degree[vi]){
                                    lardd.clear();
                                    lardd.push_back(vi);
                                }
                            }
                        }
                    }
                    if (lardd.empty()){
                        // error
                        continue;
                    } else {
                        std::uniform_int_distribution<> distr(0,lardd.size()-1);
                        int chosen = distr(rng);
                        swap(lardd[chosen],0,G,in_S,active,sizeNSk,map_degree,exp_degree,div_degree,NSk,ne_in_S,S,tab_elem,rng);
                    }
                } else {
                    int decision = flip_coin(rng);
                    if (decision){
                        std::vector<int> lardd;
                        for (const auto& vi:NSk[2]){
                            if (lardd.empty()) lardd.push_back(vi);
                            else {
                                if (div_degree[ lardd[0] ] == div_degree[vi]){
                                    lardd.push_back(vi);
                                }
                                if (div_degree[ lardd[0] ] < div_degree[vi]){
                                    lardd.clear();
                                    lardd.push_back(vi);
                                }
                            }
                        }
                        if (lardd.empty()){
                            // error
                            continue;
                        } else {
                            std::uniform_int_distribution<> distr(0,lardd.size()-1);
                            int chosen = distr(rng);
                            swap(lardd[chosen],0,G,in_S,active,sizeNSk,map_degree,exp_degree,div_degree,NSk,ne_in_S,S,tab_elem,rng);
                        }
                    } else {
                        if (NSk[3].size() == 0) continue;
                        std::uniform_int_distribution<> distr(0,NSk[3].size()-1);
                        int chosen = distr(rng);
                        auto itr = NSk[3].begin();
                        while (chosen--) itr++;
                        int node = *itr;
                        swap(node,0,G,in_S,active,sizeNSk,map_degree,exp_degree,div_degree,NSk,ne_in_S,S,tab_elem,rng);
                    }
                }
            }

        }

        if (restart == 1){
            double avg_time = ((double)(clock() - istart))/CLOCKS_PER_SEC/iters_max;
            if (time_limit > avg_time) iters_max = ceil(sqrt(time_limit/avg_time));
        }

    }
    
    return std::pair< std::vector<int>,double >(S_ans,((double)(clock() - start))/CLOCKS_PER_SEC);
}