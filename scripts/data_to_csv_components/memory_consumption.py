import numpy as np
import os
import sqlite3

def read_results(result_path, exp_type="random"):
    if not os.path.exists(result_path):
        raise FileNotFoundError("Experimental results not found!")
    global memory
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
                for line in lines:
                    if line.startswith("Memory consumption"):
                        memory[idx][idx2] = int(line.split()[-1].rstrip("MB"))

# Example data
datasets = ['lj', 'dota', 'orkut', 'g24', 'u24', 'twitter']
methods = ['Teseo', 'Sortledton', 'Spruce', 'GTX', 'RadixGraph']
memory = [
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0]
]
read_results("./results")

print("Writing memory results to CSV...")
if not os.path.exists("./csv"):
    os.makedirs("./csv")
with open("./csv/memory.csv", "w") as f:
    f.write("Method,")
    for d in datasets:
        f.write(d + ",")
    f.write("\n")
    for i, method in enumerate(methods):
        f.write(method + ",")
        for j in range(len(datasets)):
            f.write(f"{memory[i][j]:d},")
        f.write("\n")
print("Done.")