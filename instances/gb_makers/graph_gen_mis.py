import networkx as nx
import random

d = [0.001 , 0.005 , 0.01 , 0.05 , 0.1 , 0.5 , 0.9]
t = [100, 250, 500, 750, 1000]

for density in d:
    for size in t:
        for i in range(5):
            random_seed1 = random.randint(1,1000000)
            random_seed2 = random.randint(1,1000000)
            random_graph = nx.fast_gnp_random_graph(size, density, seed=random_seed1)
            random_tree = nx.random_labeled_tree(size,seed=random_seed2)
            nx_temp = nx.compose(random_graph,random_tree)
            nx_temp = nx.relabel.convert_node_labels_to_integers(nx_temp)
            graph = nx.Graph()
            graph.add_nodes_from(sorted(nx_temp.nodes()))
            graph.add_edges_from(nx_temp.edges())
            with open(f"../gb_mis/gb_mis_{size}_{density}_{i}", 'w') as file:
                
                file.write(f"# graph with density {density} created with networkx fast_gnp_random_graph with seed {random_seed1} composed with a random labeled tree with seed {random_seed2}\n")

                file.write(f"{graph.number_of_nodes()} {graph.number_of_edges()}\n")

                # Write each edge
                for edge in graph.edges():
                    node1, node2 = edge
                    file.write(f"{node1+1} {node2+1}\n")
