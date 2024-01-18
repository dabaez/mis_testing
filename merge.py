import sys
import os
import pandas as pd

if len(sys.argv) != 3:
    print("usage: merge [gb_dataset] [test_name]")
    exit()

dataset = sys.argv[1]
test_name = sys.argv[2]

data = {}

for algorithms in os.listdir("results/"+test_name):
    data[algorithms] = pd.read_csv("results/"+test_name+"/"+algorithms+"/"+dataset+".csv")

algos = list(data.keys())

df = pd.DataFrame(columns=['file'] + algos)
group_path = "instances/" + dataset
for filename in os.listdir(group_path):
    row = [filename]
    for algo in algos:
        row.append(int(data[algo].loc[(data[algo]['file'] == filename)]['result'].iloc[0]))
    df.loc[len(df.index)] = row

df.to_csv("results/" + test_name + "/" + dataset + ".csv", index=False)