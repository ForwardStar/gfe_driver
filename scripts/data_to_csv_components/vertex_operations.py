import numpy as np
import os
import sqlite3

def read_results(result_path, exp_type="vertices"):
    if not os.path.exists(result_path):
        raise FileNotFoundError("Experimental results not found!")
    global insert_throughputs
    methods = os.listdir(result_path)
    for method in methods:
        # print("Processing", method)
        method_path = os.path.join(result_path, method)
        method_path = os.path.join(method_path, exp_type)
        idx = 0
        if method == 'teseo.13':
            idx = 0
        elif method == 'sortledton.4':
            idx = 1
        elif method == 'bvgt':
            idx = 2
        elif method == 'gtx':
            idx = 3
        elif method == 'radixgraph':
            idx = 4
        else:
            continue
        for file in os.listdir(method_path):
            # print("Processing file", file)
            idx2 = 0
            if file.startswith("com-lj"):
                idx2 = 0
            elif file.startswith("dota-league"):
                idx2 = 1
            elif file.startswith("com-orkut"):
                idx2 = 2
            elif file.startswith("graph500-24"):
                idx2 = 3
            elif file.startswith("uniform-24"):
                idx2 = 4
            elif file.startswith("twitter-2010"):
                idx2 = 5
            else:
                continue
            with open(os.path.join(method_path, file), "r") as f:
                lines = f.readlines()
                m = 0
                insert_time = 0
                delete_time = 0
                for line in lines:
                    if line.startswith("Loaded"):
                        m = int(line.split()[1])
                    if line.startswith("Insertions"):
                        time_str = line.split()[6]
                        time_str = time_str.split(":")
                        multiple = 1
                        for i in range(len(time_str) - 1, -1, -1):
                            insert_time += float(time_str[i]) * multiple
                            multiple *= 60
                if insert_time == 0:
                    insert_throughputs[idx][idx2] = 0
                else:
                    insert_throughputs[idx][idx2] = m / insert_time / 1e6
                    
# Example data
datasets = ['lj', 'dota', 'orkut', 'g24', 'u24', 'twitter']
methods = ['Teseo', 'Sortledton', 'Spruce', 'GTX', 'RadixGraph']
insert_throughputs = [
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
]
read_results("./results")

print("Writing vertex throughput results to CSV...")
if not os.path.exists("./csv"):
    os.makedirs("./csv")
with open("./csv/vertex_insertion.csv", "w") as f:
    f.write("Method,")
    for d in datasets:
        f.write(d + ",")
    f.write("\n")
    for i, method in enumerate(methods):
        f.write(method + ",")
        for j in range(len(datasets)):
            f.write(f"{insert_throughputs[i][j]:.2f},")
        f.write("\n")
print("Done.")