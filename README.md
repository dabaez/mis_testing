# mis_testing
Framework for testing Maximum Independant Set problems


## Organization

### instances

Holds all of the instances for testing the problem

The file **instances_list.txt** describes each dataset and is also used for iterating through while testing

The format for each graph is:
```
# comment lines start with #
# first line is the number of nodes and edges
# then one line for each edge
# this file has 4 nodes and 5 edges
4 5
1 2
1 3
1 4
2 4
2 5
```

### classic_solvers

Holds all of the files for classic heuristics that solve the problems

In order to build the current ones (sbts, pls and ewcc) just run `make` from the root folder

To add new ones just add the command to run them to **solvers.txt** formatted so that it can receive a time limit and a file path

The last line printed to the terminal must be the Maximum Independant Set size found and the time it actually took separated by a comma

### neural_network 

Holds all of the files for the neural network to be used for comparison, you are expected to work here

To test a neural network add the command to run it in **nn_command.txt** formatted so that it can receive a time limit and a file path

The last line printed to the terminal must be the Maximum Independant Set size found and the time it actually took separated by a comma

## Codes

**run_classic.py** and **run_nn.py** are there to run all of the instances with the classic solver ot the neural network respectively. It takes a test name in order to save the different algorithms together and a time limit.

**compare_gb.py** is there to analyze the results of runs done with gb datasets. It takes the name of the dataset, the name of the test and one algorithm name in order to make a heat map out of it.

**merge.py** is for other datasets in order to merge the information of multiple algorithms into one csv. It takes a test name and a dataset name.