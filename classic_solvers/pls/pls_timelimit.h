#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <algorithm>

struct set_2a{
    std::vector<int> set_el;
    std::vector<int> index;
    set_2a(int n){
        index.assign(n,-1);
    }
    int &operator[](size_t index){
        return set_el[index];
    }
    void insert(int u){
        if (index[u] == -1){
            index[u] = set_el.size();
            set_el.emplace_back(u);
        }
    }
    void remove(int u){
        if (index[u] != -1){
            int lastp = set_el.size()-1;
            if (index[u] != lastp){
                set_el[ index[u] ] = set_el[ lastp ];
                index[ set_el[ lastp ] ] = index[u];
            }
            index[u] = -1;
            set_el.pop_back();
        }
    }
    bool empty(){
        return !(set_el.size());
    }
    int size(){
        return set_el.size();
    }
    void clear(){
        for (int x:set_el) index[x] = -1;
        set_el.clear();
    }
    bool in(int u){
        return index[u] != -1;
    }
};

int select_f(set_2a &bag,int select,std::mt19937 &rng,std::vector<int> &penalties,
           std::vector< std::vector<int> > &G){

    if (select == 0){
        std::uniform_int_distribution<> dist(0,bag.size()-1);
        return bag[dist(rng)];
    }
    if (select == 1){
        std::vector<int> mpb(1,bag[0]);
        for (int i=1;i<bag.size();i++){
            if (penalties[bag[i]] < penalties[mpb[0]]){
                mpb.clear();
                mpb.push_back(bag[i]);
            } else if (penalties[bag[i]] == penalties[mpb[0]]) mpb.push_back(bag[i]);
        }
        std::uniform_int_distribution<> dist(0,mpb.size()-1);
        return mpb[dist(rng)];
    }
    if (select == 2){
        std::vector<int> mdb(1,bag[0]);
        for (int i=1;i<bag.size();i++){
            if (G[bag[i]].size() > G[mdb[0]].size()){
                mdb.clear();
                mdb.push_back(bag[i]);
            } else if (G[bag[i]].size() == G[mdb[0]].size()) mdb.push_back(bag[i]);
        }
        std::uniform_int_distribution<> dist(0,mdb.size()-1);
        return mdb[dist(rng)];
    }
}

void calc_all(set_2a &C0U,set_2a &C0nU,set_2a &C1U,set_2a &C1nU,
              std::vector<int> &connections,set_2a &U,set_2a &K){
    
    C0U.clear(); C0nU.clear(); C1U.clear(); C1nU.clear();
    for (int i=0;i<connections.size();i++){
        if (connections[i] == K.size()){
            if (U.in(i)) C0U.insert(i);
            else C0nU.insert(i);
        } else if ( (connections[i] == K.size()-1) && (!K.in(i))){
            if (U.in(i)) C1U.insert(i);
            else C1nU.insert(i);
        }
    }

}

void phase(int iterations,int select,int perturb,int &selections, clock_t &start, double time_limit,
           set_2a &K, std::vector<int> &penalties, std::vector<int> &connections, set_2a &U,
           std::vector< std::vector<int> > &G,std::mt19937 &rng,int &tcs,
           int &cur_penalty,int &penalty_delay){

    int graph_size = G.size();
    set_2a C0U(graph_size),C0nU(graph_size),C1U(graph_size),C1nU(graph_size);
    calc_all(C0U,C0nU,C1U,C1nU,connections,U,K);

    while (iterations > 0 && ((double)(clock() - start))/CLOCKS_PER_SEC < time_limit){
        while (C0U.size() || C0nU.size() || C1nU.size()){
            while (C0U.size() || C0nU.size()){
                U.clear();
                for (int i=0;i<C0U.size();i++) C0nU.insert(C0U[i]);
                C0U.clear();
                for (int i=0;i<C1U.size();i++) C1nU.insert(C1U[i]);
                C1U.clear();

                int chosen = select_f(C0nU,select,rng,penalties,G);

                C0nU.remove(chosen);
                K.insert(chosen);

                for (int ne:G[chosen]) connections[ne]++;
                for (int i=0;i<C1nU.size();i++){
                    if (connections[C1nU[i]] != K.size()-1){
                        C1nU.remove(C1nU[i]);
                        i--;
                    }
                }

                for (int i=0;i<C0nU.size();i++){
                    if (connections[C0nU[i]] != K.size()){
                        C1nU.insert(C0nU[i]);
                        C0nU.remove(C0nU[i]);
                        i--;
                    }
                }

                selections++;
                if (K.size() == tcs) return;
            }
            if (C1nU.size()){
                int chosen = select_f(C1nU,select,rng,penalties,G);
                K.insert(chosen);
                int swap;
                for (int ne:G[chosen]) connections[ne]++;
                for (int i=0;i<K.size();i++){
                    if (connections[K[i]] != K.size()-1 && K[i] != chosen){
                        swap = K[i];
                        break;
                    }
                }
                K.remove(swap);
                for (int ne:G[swap]) connections[ne]--;
                U.insert(swap);
                calc_all(C0U,C0nU,C1U,C1nU,connections,U,K);
                selections++;
            }
        }
        iterations--;
        cur_penalty++;
        for (int i=0;i<K.size();i++) penalties[K[i]]++;
        if (cur_penalty%penalty_delay == 0){
            cur_penalty = 0;
            int count_p = 0;
            for (int i=0;i<graph_size;i++){
                if (penalties[i]) penalties[i]--;
                if (penalties[i]) count_p++;
            }
            if (count_p < G.size()*3/4) penalty_delay++;
            if (count_p > G.size()*3/4) penalty_delay = std::max(1,penalty_delay-1);
        }
        if (perturb == 0){
            std::uniform_int_distribution<> dist(0,graph_size-K.size()-1);
            int skip = dist(rng);
            int chosen = 0;
            while (true){
                if (K.in(chosen)) chosen++;
                else if (skip){
                    skip--;
                    chosen++;
                } else {
                    break;
                }
            }
            set_2a newK(graph_size);
            newK.insert(chosen);
            for (int ne:G[chosen]){
                connections[ne]++;
                if (K.in(ne)) newK.insert(ne);
            }
            for (int i=0;i<K.size();i++){
                if (!newK.in(K[i])){
                    for (int ne:G[K[i]]){
                        connections[ne]--;
                    }
                }
            }
            K = newK;
            calc_all(C0U,C0nU,C1U,C1nU,connections,U,K);
        } else {
            K.clear();
            connections.assign(graph_size,0);
            std::uniform_int_distribution<> dist(0,graph_size-1);
            int chosen = dist(rng);
            K.insert(chosen);
            for (int ne:G[chosen]) connections[ne]++;
            calc_all(C0U,C0nU,C1U,C1nU,connections,U,K);
        }
    }
}

std::vector<int> build_k(set_2a& K){
    return K.set_el;
}

std::vector<int> pls_tc(std::vector< std::vector<int> > &G, int tcs, clock_t &start, double time_limit, std::mt19937 &rng){

    int graph_size = G.size();
    std::uniform_int_distribution<> random_node(0,graph_size-1);

    int selections = 0;
    std::vector<int> penalties(graph_size,0),connections(graph_size,0);
    int cur_penalty = 0, penalty_delay = 2;
    set_2a K(graph_size),U(graph_size),U_r(graph_size),U_d(graph_size);

    int chosen = random_node(rng);
    K.insert(chosen);
    for (int ne:G[chosen]) connections[ne]++;

    std::vector<int> connections_r = connections;
    set_2a K_r = K;

    std::vector<int> connections_d = connections;
    set_2a K_d = K;

    while ( ((double)(clock() - start))/CLOCKS_PER_SEC < time_limit ){
        K = K_r;
        connections = connections_r;
        U = U_r;
        phase(50,0,0,selections,start,time_limit,K,penalties,connections,U,G,rng,tcs,cur_penalty,penalty_delay);
        if (K.size() == tcs) return build_k(K);
        K_r = K;
        connections_r = connections;
        U_r = U;

        phase(50,1,1,selections,start,time_limit,K,penalties,connections,U,G,rng,tcs,cur_penalty,penalty_delay);
        if (K.size() == tcs) return build_k(K);

        K = K_d;
        connections = connections_d;
        U = U_d;
        phase(100,2,0,selections,start,time_limit,K,penalties,connections,U,G,rng,tcs,cur_penalty,penalty_delay);
        if (K.size() == tcs) return build_k(K);
        K_d = K;
        connections_d = connections;
        U_d = U;
    }

    //std::cout<<"FAILED"<<std::endl;
    return std::vector<int>(0);

}

std::pair< std::vector<int> , double > pls(std::vector< std::vector<int> > &G, double time_limit,int seed = -1){

    if (seed == -1) seed = std::chrono::steady_clock::now().time_since_epoch().count();

    std::cout<<"Seed "<<seed<<'\n';
    std::mt19937 rng(seed);

    clock_t start = clock();
    std::vector<int> fans;

    while ( ((double)(clock() - start))/CLOCKS_PER_SEC < time_limit ){
        std::vector<int> rec = pls_tc(G,fans.size()+1,start,time_limit,rng);
        if (rec.size() > fans.size()){
            fans = rec;
        }
    }
    
    return std::pair< std::vector<int> , double >(fans,((double)(clock() - start))/CLOCKS_PER_SEC);

}