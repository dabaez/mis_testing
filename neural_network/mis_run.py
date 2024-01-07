from misf import MIS
import random
import json
import sys
import networkx as nx

filename = sys.argv[1]
runtime_limit = float(sys.argv[2])

G = nx.Graph()
with open(filename, 'r') as file:
    while True:
        line = file.readline().strip()
        if not line.startswith('#'):
            break
    num_nodes, num_edges = map(int, line.split())
    for _ in range(num_edges):
        line = file.readline().split()
        node1 = int(line[0])
        node2 = int(line[1])
        G.add_edge(node1, node2)
G = nx.relabel.convert_node_labels_to_integers(G)
nx_graph = nx.OrderedGraph()
nx_graph.add_nodes_from(sorted(G.nodes()))
nx_graph.add_edges_from(G.edges)

time, mis = MIS(nx_graph, runtime_limit , random.randint(1,1000000))

print(str(mis) + "," + str(time))