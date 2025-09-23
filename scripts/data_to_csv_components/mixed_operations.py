import numpy as np
import os
import sqlite3

def read_results(result_path):
    if not os.path.exists(result_path):
        raise FileNotFoundError("Experimental results not found!")
    
    global throughput_g500
    global throughput_u24
    methods = os.listdir(result_path)
    for method in methods:
        print("Processing", method)
        method_path = os.path.join(result_path, method)
        method_path = os.path.join(method_path, 'mixed')
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

        if not os.path.exists(method_path):
            continue

        for file in os.listdir(method_path):
            if not file.endswith('sqlite3'):
                continue
            print("Processing file", file)

            # Connect to the SQLite database
            conn = sqlite3.connect(os.path.join(method_path, file))
            cursor = conn.cursor()

            # Load time and ops from the table
            cursor.execute("SELECT * FROM aging_intermediate_throughput3")
            rows = cursor.fetchall()

            # Extract time (3rd col) and cumulative ops (4th col)
            times = [row[2] for row in rows]
            ops = [row[3] for row in rows]

            # Total operations
            total_ops = ops[-1]
            ten_percent = total_ops // 10
            if method == 'gtx':
                # We only execute 20% operations for GTX (cause it raises OOM for more operations)
                # If you executed the full operations with a large-memory machine, comment this
                ten_percent *= 5

            # Compute throughput every 10%
            result = []
            current_threshold = ten_percent

            for i in range(1, len(ops)):
                if ops[i] >= current_threshold:
                    # Time and ops between checkpoints
                    result.append(times[i])
                    
                    current_threshold += ten_percent

                    # Stop if we’ve reached 100%
                    if current_threshold > total_ops:
                        break

            if file.startswith("graph500-24"):
                throughput_g500[idx] = result[:]
            elif file.startswith("uniform-24"):
                throughput_u24[idx] = result[:]
            # Close DB connection
            conn.close()
            
methods = ['Teseo', 'Sortledton', 'Spruce', 'GTX', 'RadixGraph']
throughput_g500 = [
    [],
    [],
    [],
    [],
    []
]
throughput_u24 = [
    [],
    [],
    [],
    [],
    []
]
read_results("./results")

print("Writing mixed update throughput results to CSV...")
if not os.path.exists("./csv"):
    os.makedirs("./csv")
with open("./csv/mixed_graph500.csv", "w") as f:
    f.write("Method,")
    for j in range(10):
        f.write(f"{(j+1)*10}%,")
    f.write("\n")
    for i, method in enumerate(methods):
        f.write(method + ",")
        for j in range(10):
            if j < len(throughput_g500[i]):
                f.write(f"{throughput_g500[i][j]:.2f},")
            else:
                f.write("0,")
        f.write("\n")
with open("./csv/mixed_uniform24.csv", "w") as f:
    f.write("Method,")
    for j in range(10):
        f.write(f"{(j+1)*10}%,")
    f.write("\n")
    for i, method in enumerate(methods):
        f.write(method + ",")
        for j in range(10):
            if j < len(throughput_u24[i]):
                f.write(f"{throughput_u24[i][j]:.2f},")
            else:
                f.write("0,")
        f.write("\n")
print("Done.")