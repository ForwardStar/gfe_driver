import numpy as np
import os
import sqlite3

def read_results(result_path):
    if not os.path.exists(result_path):
        raise FileNotFoundError("Experimental results not found!")
    global get_neighbor_throughputs
    global two_hop_throughputs
    global bfs_latency
    global sssp_latency
    global pr_latency
    global wcc_latency
    methods = os.listdir(result_path)
    for method in methods:
        # print("Processing", method)
        method_path = os.path.join(result_path, method)
        method_path = os.path.join(method_path, "analytics")
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
            elif file.startswith("twitter"):
                idx2 = 5
            else:
                continue
            lcc_timed_out = False
            with open(os.path.join(method_path, file), "r") as f:
                lines = f.readlines()
                n = 0
                for line in lines:
                    if line.startswith("Loaded"):
                        n = int(line.split()[6].rstrip('.')) + 1
                    if line.startswith("Get 1-hop"):
                        try:
                            n = int(line.split()[4])
                        except:
                            pass
                        tm = line.split()[8]
                        if line.split()[-1] == "ms":
                            get_neighbor_throughputs[idx][idx2] = int(tm) / 1000
                        else:
                            tm = tm.split(":")
                            multiple = 1
                            for i in range(len(tm) - 1, -1, -1):
                                get_neighbor_throughputs[idx][idx2] += float(tm[i]) * multiple
                                multiple *= 60
                        get_neighbor_throughputs[idx][idx2] = n / get_neighbor_throughputs[idx][idx2]
                    if line.startswith("Get 2-hop"):
                        try:
                            n = int(line.split()[4])
                        except:
                            pass
                        tm = line.split()[8]
                        if line.split()[-1] == "ms":
                            two_hop_throughputs[idx][idx2] = int(tm) / 1000
                        else:
                            tm = tm.split(":")
                            multiple = 1
                            for i in range(len(tm) - 1, -1, -1):
                                two_hop_throughputs[idx][idx2] += float(tm[i]) * multiple
                                multiple *= 60
                        two_hop_throughputs[idx][idx2] = n / two_hop_throughputs[idx][idx2]
                    if line.startswith(">> BFS N:"):
                        tm = line.split()[5].rstrip(",")
                        bfs_latency[idx][idx2] = int(tm) / 1000
                    if line.startswith(">> SSSP N:"):
                        tm = line.split()[5].rstrip(",")
                        sssp_latency[idx][idx2] = int(tm) / 1000
                    if line.startswith(">> PageRank N:"):
                        tm = line.split()[5].rstrip(",")
                        pr_latency[idx][idx2] = int(tm) / 1000
                    if line.startswith(">> WCC N:"):
                        tm = line.split()[5].rstrip(",")
                        wcc_latency[idx][idx2] = int(tm) / 1000
                    if "LCC timed out" in line:
                        lcc_timed_out = True
                    if line.startswith(">> LCC N:"):
                        if not lcc_timed_out:
                            tm = line.split()[5].rstrip(",")
                            lcc_latency[idx][idx2] = int(tm) / 1000
                    if line.startswith(">> BC N"):
                        tm = line.split()[5].rstrip(",")
                        bc_latency[idx][idx2] = int(tm) / 1000



# Example data
datasets = ['lj', 'dota', 'orkut', 'g24', 'u24', 'twitter']
methods = ['Teseo', 'Sortledton', 'Spruce', 'GTX', 'RadixGraph']
get_neighbor_throughputs = [
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0]
]
two_hop_throughputs = [
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0]
]
bfs_latency = [
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0]
]
sssp_latency = [
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0]
]
pr_latency = [
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0]
]
wcc_latency = [
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0]
]
lcc_latency = [
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0]
]
bc_latency = [
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0]
]
read_results("./results")

print("Writing analytics results to CSV...")
if not os.path.exists("./csv"):
    os.makedirs("./csv")
with open("./csv/get_neighbor_throughput.csv", "w") as f:
    f.write("Method,")
    for d in datasets:
        f.write(d + ",")
    f.write("\n")
    for i, method in enumerate(methods):
        f.write(method + ",")
        for j in range(len(datasets)):
            f.write(f"{get_neighbor_throughputs[i][j]:.2f},")
        f.write("\n")
with open("./csv/two_hop_throughput.csv", "w") as f:
    f.write("Method,")
    for d in datasets:
        f.write(d + ",")
    f.write("\n")
    for i, method in enumerate(methods):
        f.write(method + ",")
        for j in range(len(datasets)):
            f.write(f"{two_hop_throughputs[i][j]:.2f},")
        f.write("\n")
with open("./csv/bfs_latency.csv", "w") as f:
    f.write("Method,")
    for d in datasets:
        f.write(d + ",")
    f.write("\n")
    for i, method in enumerate(methods):
        f.write(method + ",")
        for j in range(len(datasets)):
            f.write(f"{bfs_latency[i][j]:.2f},")
        f.write("\n")
with open("./csv/sssp_latency.csv", "w") as f:
    f.write("Method,")
    for d in datasets:
        f.write(d + ",")
    f.write("\n")
    for i, method in enumerate(methods):
        f.write(method + ",")
        for j in range(len(datasets)):
            f.write(f"{sssp_latency[i][j]:.2f},")
        f.write("\n")
with open("./csv/pr_latency.csv", "w") as f:
    f.write("Method,")
    for d in datasets:
        f.write(d + ",")
    f.write("\n")
    for i, method in enumerate(methods):
        f.write(method + ",")
        for j in range(len(datasets)):
            f.write(f"{pr_latency[i][j]:.2f},")
        f.write("\n")
with open("./csv/wcc_latency.csv", "w") as f:
    f.write("Method,")
    for d in datasets:
        f.write(d + ",")
    f.write("\n")
    for i, method in enumerate(methods):
        f.write(method + ",")
        for j in range(len(datasets)):
            f.write(f"{wcc_latency[i][j]:.2f},")
        f.write("\n")
with open("./csv/bc_latency.csv", "w") as f:
    f.write("Method,")
    for d in datasets:
        f.write(d + ",")
    f.write("\n")
    for i, method in enumerate(methods):
        f.write(method + ",")
        for j in range(len(datasets)):
            f.write(f"{bc_latency[i][j]:.2f},")
        f.write("\n")
with open("./csv/lcc_latency.csv", "w") as f:
    f.write("Method,")
    for d in datasets:
        f.write(d + ",")
    f.write("\n")
    for i, method in enumerate(methods):
        f.write(method + ",")
        for j in range(len(datasets)):
            f.write(f"{lcc_latency[i][j]:.2f},")
        f.write("\n")
print("Done.")