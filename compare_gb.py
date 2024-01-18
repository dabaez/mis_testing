import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
import sys
import os
import pandas as pd
import numpy as np

if len(sys.argv) != 4:
    print("usage: compare_gb [gb_dataset] [test_name] [main_alg]")
    exit()

dataset = sys.argv[1]
test_name = sys.argv[2]
main_alg = sys.argv[3]

sizes = [100,250,500,750,1000]
densities = [0.001 , 0.005 , 0.01 , 0.05 , 0.1 , 0.5 , 0.9]
data = {}

for algorithms in os.listdir("results/"+test_name):
    data[algorithms] = pd.read_csv("results/"+test_name+"/"+algorithms+"/"+dataset+".csv")

fig, axs = plt.subplots(1, len(sizes), figsize=(25, 5))

algos = data.keys()

bar_width = 0.125
index = np.arange(len(densities))
axs[0].set_xlabel("Density")
axs[0].set_ylabel("Percentage of the best MIS found")

colors = []

for i in range(len(sizes)):
    d_percentages = {algo: [] for algo in algos}
    for density in densities:
        percentages = {algo: 0 for algo in algos}
        for j in range(5):
            filename = dataset + "_" + str(sizes[i]) + "_" + str(density) + "_" + str(j)
            best = 0
            for algo in algos:
                best = max(best,data[algo].loc[(data[algo]['file'] == filename)]["result"].iloc[0])
            #print(f"size {sizes[i]}, density {density} {j} : {best}")
            for algo in algos:
                percentages[algo] += data[algo].loc[(data[algo]['file'] == filename)]["result"].iloc[0]/best
        for algo in algos:
            percentages[algo]/=5
            d_percentages[algo].append(percentages[algo])
    # Create the grouped bar plot using a for loop
    for j, (algo, percentages) in enumerate(d_percentages.items()):
        axs[i].bar(index + j * bar_width, percentages, bar_width, label=algo)
    axs[i].set_xticks(index + bar_width * (len(algos) - 1) / 2, densities)
    axs[i].yaxis.set_major_formatter(mtick.PercentFormatter(xmax=1.0))
    axs[i].set_title(f'Graph size {sizes[i]}')
    if (i != 0):
        axs[i].set_yticklabels([])
    else:
        axs[i].legend(bbox_to_anchor=(0, 1.5), loc='upper left', ncols=2)
    colors.append(d_percentages[main_alg])

plt.subplots_adjust(top=0.7)
plt.show()

plt.close()

for i in range(len(colors)):
    for j in range(len(colors[i])):
        colors[i][j] *= 100

d_range = [i for i in range(len(densities))]
s_range = [i for i in range(len(sizes))]
plt.pcolormesh(d_range, s_range, colors, cmap='viridis')
plt.yticks(s_range,[str(value) for value in sizes])
plt.xticks(d_range,[str(value) for value in densities])
plt.ylabel("Size")
plt.xlabel("Density")
cbar = plt.colorbar()
cbar.set_label('Percentage of the best MIS found')

for i in range(len(sizes)):
    for j in range(len(densities)):
        plt.text(j, i, f'{colors[i][j]:.2f}%', ha='center', va='center', color='white')

plt.show()