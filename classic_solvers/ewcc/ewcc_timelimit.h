#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <set>

struct Node{
    int data;
    Node *next, *prev;
    Node(int val){
        data = val;
        next = NULL;
        prev = NULL;
    }
};

struct LinkedList{
    Node *head;
    int size;
    LinkedList(){
        head = NULL;
        size = 0;
    }
    void insertNode(int val){
        Node *newNode = new Node(val);
        newNode->next = head;
        if (head != NULL) head->prev = newNode;
        head = newNode;
        size++;
    }
    Node *removeNode(Node *p){
        size--;
        if (p == head) head = head->next;
        if (p->prev != NULL) p->prev->next = p->next;
        if (p->next != NULL) p->next->prev = p->prev;
        Node *ret = p->next;
        delete p;
        return ret;
    }
};

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

void del_node(LinkedList &L, Node *&UL, int v, std::vector< std::pair<int,int> > &edg){
    
    Node *cp = L.head;
    while (cp->data != -1){
        if (edg[ cp->data ].first == v || edg[ cp->data ].second == v){
            if (UL == cp){
                UL = cp->next;
            }
            cp = L.removeNode(cp);
        } else {
            cp = cp->next;
        }
    }

}

std::vector<int> build_c(set_2a &C){
    return C.set_el;
}

int choose_random_max_dscore(set_2a &bag,std::vector<int> &dscore,std::mt19937 &rng){
    std::vector<int> pos(1,bag[0]);
    for (int i=1;i<bag.size();i++){
        if (dscore[pos[0]] < dscore[bag[i]]){
            pos.clear();
            pos.push_back(bag[i]);
        } else if (dscore[pos[0]] == dscore[bag[i]]){
            pos.push_back(bag[i]);
        }
    }
    std::uniform_int_distribution<> dist(0,pos.size()-1);
    return pos[dist(rng)];
}

void build_s(int v,set_2a &C,int tabu_remove,std::vector<int> &dscore,std::vector< std::pair<int,int> > &S,
             std::vector< std::vector< std::pair<int,int> > > &Gix,std::vector<int> &edgw){
    std::vector<int> scores(C.size());
    for (int i=0;i<C.size();i++) scores[i] = dscore[v] + dscore[C[i]];
    for (int i=0;i<Gix[v].size();i++){
        if (C.in(Gix[v][i].first)){
            scores[ C.index[ Gix[v][i].first ] ] += edgw[ Gix[v][i].second ];
        }
    }
    for (int i=0;i<C.size();i++){
        if (C[i] != tabu_remove && scores[i] > 0){
            S.emplace_back(C[i],v);
        }
    }
}

std::pair< std::vector<int> , double > ewcc(std::vector< std::vector<int> > &G, double time_limit,int seed = -1){

    clock_t start = clock();

    if (seed == -1) seed = std::chrono::steady_clock::now().time_since_epoch().count();

    std::cout<<"Seed "<<seed<<'\n';
    std::mt19937 rng(seed);

    int graph_size = G.size();

    std::vector< std::pair<int,int> > edg;
    for (int i=0;i<graph_size;i++){
        for (int ne:G[i]){
            if (i < ne) edg.emplace_back(i,ne);
        }
    }

    std::vector< std::vector< std::pair<int,int> > > Gix(graph_size);
    for (int i=0;i<edg.size();i++){
        Gix[ edg[i].first ].emplace_back(edg[i].second,i);
        Gix[ edg[i].second ].emplace_back(edg[i].first,i);
    }

    int step = 0;
    int tabu_remove = -1;

    LinkedList L;
    L.insertNode(-1);
    Node *UL = L.head;
    Node *tail = L.head;

    for (int i=0;i<edg.size();i++) L.insertNode(i);

    std::vector<int> edgw(edg.size(),1);

    std::vector<int> dscore(graph_size);
    for (int i=0;i<graph_size;i++) dscore[i] = G[i].size();

    std::vector<int> confChange(graph_size,1);

    set_2a C(graph_size);

    set_2a Cc(graph_size);
    for (int i=0;i<graph_size;i++) Cc.insert(i);

    while (L.size > 1){
        int chosen = choose_random_max_dscore(Cc,dscore,rng);
        Cc.remove(chosen);
        C.insert(chosen);
        for (int ne:G[chosen]){
            if (C.in(ne)) dscore[ne]++;
            else dscore[ne]--;
        }
        dscore[chosen] = -dscore[chosen];
        del_node(L,UL,chosen,edg);
    }

    std::vector<int> C_opt;

    while (L.size == 1){
        C_opt = build_c(C);
        int chosen = choose_random_max_dscore(C,dscore,rng);
        C.remove(chosen);
        for (const auto& [ne,edgi]:Gix[chosen]){
            if (C.in(ne)) dscore[ne]--;
            else {
                dscore[ne]++;
                L.insertNode(edgi);
            }
        }
        dscore[chosen] = -dscore[chosen];
    }

    while ( ((double)(clock() - start))/CLOCKS_PER_SEC < time_limit ){

        int u=-1,v=-1;
        std::vector< std::pair<int,int> > S;
        std::pair<int,int> e = edg[tail->prev->data];
        if (confChange[e.first]) build_s(e.first,C,tabu_remove,dscore,S,Gix,edgw);
        if (confChange[e.second]) build_s(e.second,C,tabu_remove,dscore,S,Gix,edgw);
        if (S.size()){
            std::uniform_int_distribution<> dist(0,S.size()-1);
            int chosen = dist(rng);
            u = S[chosen].first;
            v = S[chosen].second;
        } else {
            while (UL != L.head){
                UL = UL->prev;
                std::pair<int,int> e = edg[UL->data];
                if (confChange[e.first]) build_s(e.first,C,tabu_remove,dscore,S,Gix,edgw);
                if (confChange[e.second]) build_s(e.second,C,tabu_remove,dscore,S,Gix,edgw);
                if (S.size()){
                    std::uniform_int_distribution<> dist(0,S.size()-1);
                    int chosen = dist(rng);
                    u = S[chosen].first;
                    v = S[chosen].second;
                    break;
                }
            }
        }
        if (u == -1){
            Node *rec = L.head;
            while (rec->data != -1){
                edgw[rec->data]++;
                confChange[ edg[rec->data].first ] = 1;
                confChange[ edg[rec->data].second ] = 1;
                rec = rec->next;
            }
            std::uniform_int_distribution<> dist1(0,C.size()-1);
            int chosen = dist1(rng);
            u = C[ chosen ];
            std::uniform_int_distribution<> dist2(0,L.size-2);
            int skip = dist2(rng);
            rec = L.head;
            while (skip--) rec = rec->next;
            int edgv = rec->data;
            std::uniform_int_distribution<> dist3(0,1);
            int flip = dist3(rng);
            if (flip == 0) v = edg[edgv].first;
            else v = edg[edgv].second;
        }

        C.insert(v);
        for (const auto& [ne,edgi]:Gix[v]){
            if (C.in(ne)) dscore[ne]+=edgw[edgi];
            else {
                confChange[ne] = 1;
                dscore[ne]-=edgw[edgi];
            }
        }
        dscore[v] = -dscore[v];
        del_node(L,UL,v,edg);
        C.remove(u);
        for (const auto& [ne,edgi]:Gix[u]){
            if (C.in(ne)) dscore[ne]-=edgw[edgi];
            else {
                confChange[ne] = 1;
                dscore[ne]+=edgw[edgi];
                L.insertNode(edgi);
            }
        }
        confChange[u] = 0;
        dscore[u] = -dscore[u];

        tabu_remove = v;

        while (L.size == 1){
            C_opt = build_c(C);
            int chosen = choose_random_max_dscore(C,dscore,rng);
            C.remove(chosen);
            for (const auto& [ne,edgi]:Gix[chosen]){
                if (C.in(ne)) dscore[ne]--;
                else {
                    dscore[ne]++;
                    L.insertNode(edgi);
                }
            }
            dscore[chosen] = -dscore[chosen];
        }
        
        step++;
    }

    return std::pair<std::vector<int>,double>(C_opt,((double)(clock() - start))/CLOCKS_PER_SEC);
}