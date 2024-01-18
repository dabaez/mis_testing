import subprocess
import sys
import os
import pandas as pd

if len(sys.argv) != 3:
    print("usage: run_nn [test_name] [time_limit]")
    exit()

test_name = sys.argv[1]
time_limit = sys.argv[2]

os.makedirs("results/" + test_name, exist_ok=True)


with open('neural_network/nn_command.txt', 'r') as file:
    commands = [line.strip() for line in file]

with open('instances/instances_list.txt', 'r') as file:
    instance_groups = [line.strip() for line in file]

for command_full in commands:
    if command_full.startswith("#"):
        continue
    name, command = command_full.split(':', 1)
    print("testing " + name)
    folder_path = "results/"+test_name+"/"+name
    os.makedirs(folder_path,exist_ok=True)
    for instance_group in instance_groups:
        if instance_group.startswith("#"):
            continue
        group_name, group_description = instance_group.split(':',1)
        print("testing " + name + " with " + group_name)
        csv_path = folder_path +"/" + group_name + ".csv"
        if os.path.exists(csv_path):
            df = pd.read_csv(csv_path)
        else:
            df = pd.DataFrame(columns=['file', 'time', 'result'])
            df.to_csv(csv_path, index=False)
        group_path = "instances/" + group_name
        for filename in os.listdir(group_path):
            if not df.loc[(df['file'] == filename)].empty:
                continue
            file_path = "../" + group_path + "/" + filename
            formatted_command = command.format(file_path,time_limit)
            result = subprocess.run(formatted_command, shell=True, capture_output=True,cwd="neural_network/")
            print(result.stdout.decode('utf-8'))
            print(result.stderr.decode('utf-8'))
            result_text = result.stdout.decode('utf-8').splitlines()[-1].split(',')
            mis = int(result_text[0])
            time = float(result_text[1])
            df.loc[len(df.index)] = [filename, time, mis]
            df.to_csv(csv_path, index=False)