---
GFE Driver for RadixGraph
---

<!-- The GFE (Graph Framework Evaluation) Driver is the program used to run the experiments in "Spruce: a Fast yet Space-saving Structure for Dynamic Graph Storage", measuring the throughput of updates in libraries supporting structural dynamic graphs and the completion times of the [Graphalytics kernels](https://github.com/ldbc/ldbc_graphalytics).  -->
The driver supports the following structures: [RadixGraph](https://github.com/ForwardStar/RadixGraph), [GTX](https://github.com/Jiboxiake/GTX-SIGMOD2025?tab=readme-ov-file), [Spruce](https://github.com/Stardust-SJF/gfe_driver/tree/v2.0.0), [Sortledton](https://gitlab.db.in.tum.de/per.fuchs/sortledton), [Teseo](https://github.com/cwida/teseo), [GraphOne](https://github.com/the-data-lab/GraphOne), 
[Stinger](http://stingergraph.com/) and [LiveGraph](https://github.com/thu-pacman/LiveGraph-Binary). 
It can run several kinds experiments: insert/delete all edges in a random permuted order from an input graph, 
execute the updates specified by a [graphlog file](https://github.com/whatsthecraic/graphlog) and run the kernels of the Graphalytics suite: BFS, PageRank (PR), local triangle counting (LCC), weighted shortest paths (SSSP), weakly connected components (WCC) and community detection through label propagation (CDLP).

### Build 

#### Requisites 
- O.S. Linux
- Autotools, [Autoconf 2.69+](https://www.gnu.org/software/autoconf/)
- CMake 3.24 +
- A C++17 compliant compiler with support for OpenMP. We tested it with GCC 11.4.0.
- libnuma 2.0 +
- [zlib 1.3.1 +](https://github.com/madler/zlib)
- [libpapi 5.5 +](http://icl.utk.edu/papi/)
- [SQLite 3.27 +](https://sqlite.org)
- [Intel Threading Building Blocks](https://github.com/uxlfoundation/oneTBB) (version 2022.01)
- libevent 2.1.x
- libboost
- jemalloc 5.2.1+
- Disable NUMA balancing feature to avoid the Linux Kernel to swap pages during insertions: `echo 0 | sudo tee  /proc/sys/kernel/numa_balancing`
- Python 3.5+ for downloading datasets (Or you can download manually)

If you are root on an ubuntu machine, you can install the prerequisites via:
```sh
sudo apt install gcc-11 libnuma-dev cmake zlib1g-dev libpapi-dev libjemalloc-dev python3 libtbb-dev libevent-dev libboost-all-dev
```

#### Configure

After cloning this repository, initialise the sources and the configure script by:

```sh
git submodule update --init
mkdir build && cd build
autoreconf -iv ..
```

For the rest of the configuration part, note that you need to reconfigure it for each time you evaluate a different method.

##### RadixGraph

We added Git submodule of RadixGraph in ``library/radixgraph/RadixGraph``. You will need to fetch from [upstream](https://github.com/ForwardStar/RadixGraph) and compile the codes to a library. For this paper, we evaluated commit "804c9a0d027901b1c9f1dc9bc91c171bc4096612".
```sh
cd library/radixgraph/RadixGraph
git submodule update --init --recursive
cmake -S . -DCMAKE_BUILD_TYPE=Release
make
```

This would generate ``libRG.a``. Also, compile the dynamic programming optimizer:
```sh
/path/to/compiler optimizer.cpp -o optimizer -O3
```

Then move them to the root directory:
```sh
mv libRG.a ../../../
mv optimizer ../../../
cd ../../../
```

Now you can configure the driver with:

```sh
cd build
../configure --enable-optimize --disable-debug --with-radixgraph=../
```

##### Stinger
Use the branch `feature/gfe `, it contains additional patches w.r.t. 
[upstream](https://github.com/stingergraph/stinger), from https://github.com/whatsthecraic/stinger.
For the paper, we evaluated commit "2bcfac38785081c7140b0cd27f3aecace088d664"

```sh
git clone https://github.com/whatsthecraic/stinger -b feature/gfe
cd stinger
mkdir build && cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=0 
make
```
If the build has been successful, it should at least create the executable `bin/stinger_server`.

Configure the GFE driver with:

```sh
cd build
../configure --enable-optimize --disable-debug --with-stinger=/path/to/stinger/build
```

It is noted that you can build stinger in any directory, and only need to set the corresponding build directory when cofiguring gfe_driver.

##### GraphOne

Use the branch `feature/gfe `, it contains additional patches w.r.t.
[upstream](https://github.com/the-data-lab/GraphOne), from https://github.com/whatsthecraic/GraphOne.
For the paper, we evaluated "1475bf5887aaf37dd7aa47377e9f11a94aa0d880".

```sh
git clone https://github.com/whatsthecraic/GraphOne -b feature/gfe
cd GraphOne
mkdir build && cd build
cmake -S ../ -DCMAKE_BUILD_TYPE=Release
make -j
```
If the build has been successful, it should at least create the executable `graphone64`.
Then, configure the driver with:

```sh
cd build
../configure --enable-optimize --disable-debug --with-graphone=/path/to/graphone/build
```

##### LiveGraph

Download the binary library from the [official repository](https://github.com/thu-pacman/LiveGraph-Binary/releases). 
In the paper, we evaluated version 20200829. You need to download `liblivegraph.tar.gz` and extract `liblivegraph.so` into a folder.
Then configure the driver by pointing the path to where the library has been extracted:

```sh
cd build
../configure --enable-optimize --disable-debug --with-livegraph=/path/to/livegraph/lib
```

LiveGraph requires older version of TBB. You can install by:
```sh
sudo apt install libtbb2=2020.3-1
sudo apt install libtbb-dev=2020.3-1
```

Alternatively, install the older versioned TBB locally and add following to the generated Makefile:
```sh
LDFLAGS += -I${TBB_PATH}/include -L${TBB_PATH}/lib -ltbb
```

##### Teseo

Use the branch `master` from https://github.com/cwida/teseo.
In the paper, we evaluated version `14227577731d6369b5366613f3e4a679b1fd7694`.

```sh
git clone https://github.com/cwida/teseo
cd teseo
autoreconf -iv
mkdir build && cd build
../configure --enable-optimize --disable-debug
make -j
```

If the build has been successful, it should at least create the archive `libteseo.a`.
Then configure the driver with:

```sh
cd build
../configure --enable-optimize --disable-debug --with-teseo=/path/to/teseo/build   
```

##### Sortledton
Use the branch `master` from `https://gitlab.db.in.tum.de/per.fuchs/sortledton`.
For the paper, we evaluated commit "a32b8ac208bb889b518e14b1317957c9a8c466b6".

Follow the instructions in the README of the repository to setup and build the library.
Then configure the driver with:

```sh
cd build
../configure --enable-optimize --disable-debug --with-sortledton=/path/to/sortledton/build  
```

##### Spruce

Download the library `libBVGT_stable.a` from `https://github.com/Stardust-SJF/gfe_driver/releases/tag/v2.0.0` and rename it to `libBVGT.a`. Then you can configure the driver with:

```sh
cd build
../configure --enable-optimize --disable-debug --with-bvgt=/path/to/spruce/build/
```

Spruce requires [junction](https://github.com/preshing/junction). You need to compile an ``include`` and ``lib`` folder following the instructions of junction. After you run the above configure commands and generate a ``Makefile`` in the ``build`` folder, locate the ``SRC_CPPFLAGS`` and ``LDFLAGS`` and add following to the Makefile:
```
...
SRC_CPPFLAGS := ...
SRC_CPPFLAGS += -I/path/to/junction/include
...
LDFLAGS := ...
LDFLAGS += -L/path/to/junction/lib -I/path/to/junction/include 
```

##### GTX
Currently we use the branch 'master' from 'https://github.com/Jiboxiake/GTX-SIGMOD2025' . If GFE_DRIVER is used to reproduce the experiments for billion-edges grahs, please change the [USING_BIGDATA] flag to true in ./core/graph_global.hpp. Follow the instruction in REAME to build GTX. After GTX has been built, configure the driver with:
```
cd build
../configure --enable-optimize --disable-debug --with-gtx=/path/to/GTX/build
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/GTX/build
export LD_LIBRARY_PATH 
```

#### Compile

Once configured, run `make clean && make -j`. Due to name collision, you cannot run `make -j` directly. Instead, run `make clean` every time before you compile. There is no `install` target, the final artifact is the executable `gfe_driver`.

### Datasets

In our experiments, we used the following input graphs and data sets:

- `dota-league` and `graph500-SF`, with `SF` in {24 26} , were taken from the [official Graphalytics collection](https://www.graphalytics.org/datasets).
- `uniform-SF`, with `SF` in {24, 26} were generated with an [ad-hoc tool](https://github.com/whatsthecraic/uniform_graph_generator). These are synthetic graphs having the same number of vertices and edges of `graph500-SF`, but a uniform node degree distribution.
- `livejournal`, `orkut`, `twitter` and `com-friendster` were taken from the [Stanford Large Network Dataset Collection](https://snap.stanford.edu/data/index.html).

A complete image of all datasets used in the experiments can be downloaded here: [dota-league, graph500-SF, and uniform-SF](https://zenodo.org/record/3966439), [livejournal, orkut, twitter, and friendster](https://snap.stanford.edu/data/index.html).

Or you can simply download all datasets by (requires some time to download and extract the datasets):
```sh
python3 downloader.py
```

For `livejournal`, `orkut`, `twitter` and `com-friendster`, you need to remove the comment lines and reformat them before running:
```sh
sed -i '/^#/d' datasets/*.txt
sed -i -E 's/[[:space:]]+/ /g' datasets/*.txt
mv datasets/twitter-2010.txt datasets/twitter-2010.el
mv datasets/com-friendster.ungraph.txt datasets/com-friendster.ungraph.el
mv datasets/com-lj.ungraph.txt datasets/com-lj.ungraph.el
mv datasets/com-orkut.ungraph.txt datasets/com-orkut.ungraph.el
```

#### About dataset format:
GFE driver supports two types of graph formats: 
1. Plain edge list (ends with ".el"): The file contains pairs of vertex IDs, with each edge represented by a line of two space-separated integers indicating connections.
2. Edge-Vertex Separated format, which contains 3 files:
   - `graph.properties`: A file contains graph properties, including filenames for the vertex and edge files, the number of vertices and edges, whether the graph is directed, and specific parameters for running graph algorithms.
   - `graph.v`: A list of unique vertices present in the graph, with each line in the file representing a unique vertex ID.
   - `graph.e`: A list of edges from the graph, with each line representing a edge in the format `from_node_id to_node_id`.

To convert a plain edge list file ``.el`` to the separated format, you can use the provided script:
```sh
g++ generate_property_files.cpp -o generate_property_files -O3
./generate_property_files /path/to/graph.el
```

This will generate corresponding ``.v``, ``.e`` and ``.properties`` files in the same directory as the input file.

### Executing the driver

The driver takes as input a list of options together with a graph, and emits the results into a sqlite3 database. We noted that several graph systems can raise OOM exceptions on a 256GB machine.

For `RadixGraph`, we need to compute an optimal setting for the radix tree, so you need to execute `optimizer` before running the gfe_driver:
```
./optimizer <dataset-path>.<v/el/txt> <#layers> > settings
```

There are three kinds of experiments that can be executed:

- **Random Insertions and Deletions** : insert all vertices and edges from an input graph in random order; then delete all inserted edges in random order.
```
./gfe_driver -G /path/to/input/graph.properties -u -l <system_to_evaluate> -w <num_threads> -d output_results.sqlite3
```

- **Sequential Insertions and Deletions**: add ``--is_timestamped true`` option to insert or delete the edges in a timestamped order.
```
./gfe_driver -G /path/to/input/graph.properties -u -l <system_to_evaluate> -w <num_threads> -d output_results.sqlite3 --is_timestamped true
```

Here are some examples:

If you are using a property file to indicate the input graph:
```
./gfe_driver -u -G ../../../../Dataset/GFEDataset/graph500-24.properties -l radixgraph -w 28 -d extra20230224.sqlite3
```
If you are using a plain edge list file (each line of the file contains two integers) as the input graph:
```
./gfe_driver -u -G ../../../../Dataset/GFEDataset/yahoo-song.el -l radixgraph -w 56 --is_timestamped true -d extra20230224.sqlite3
```

- **Vertex Insertions Only**: prepare a vertex file and add ``--insert_vertex_only true`` option before executing gfe_driver. The vertex file should be in the following format and its filename should be ended with ``.vertices.el``:
```
u1 u2
u2 u3
u3 u4
u4 u1
```

for vertices ``u1, u2, u3, u4``. Then run:
```
./gfe_driver -G /path/to/input/graph.vertices.el -u -l <system_to_evaluate> -w <num_threads> -d output_results.sqlite3
```

- **Updates**: perform all insertions and deletions from a log. Add the option --log /path/to/updates.graphlog :

```
./gfe_driver -G /path/to/input/graph.properties -u --log /path/to/updates.graphlog --aging_timeout 24h -l <system_to_evaluate> -w <num_threads> -d output_results.sqlite3
```

Following parameters can be used to record memory footprint during the update process:

```shell
-aging_memfp --aging_memfp_physical --aging_memfp_threshold 500G --aging_release_memory=false
```

The option `--aging_timeout` serves to limit the total time to execute the experiment.
The option `--aging_memfp` records the memory footprint as the experiment proceeds.
The option `--aging_memfp_physical` records the physical memory (RSS) of the process rather than the virtual memory of the glibc allocator. 
The option`--aging_memfp_threshold 500G` terminates the experiment if the memory footprint measured is greater than 330 GB .
The option `--aging_release_memory=false` avoids releasing the memory used in the driver to load the graph from the file, as it may (or may not) recycled by the libraries. 

- **Graph anlytics**: Add the option `-R <N>` to repeat `N` times the execution of all Graphalytics kernels, one after the other. E.g., to run the kernels five times, after all vertices and edges have been inserted, use:

```
./gfe_driver -G /path/to/input/graph.properties -u -l <system_to_evaluate> -w <num_threads> -R 5 -d output_results.sqlite3
```

Type `./gfe_driver -h` for the full list of options and for the libraries that can be evaluated (option `-l`). The driver spawns the number of threads given by the option `-w` to concurrently run all insertions or updates. For Graphalytics, it defaults to the total number of the physical threads in the machine. This setting can be changed with the option `-r <num_threads>`. Note that the numbers
in the library codes (e.g. teseo.**6**, stinger**3**) are unrelated to the versions of the systems evaluated, they were only used
internally for development purposes.

The database `output_results.sqlite3` will contain the final results. 

### Repeating the experiments

These are the full commands to repeat the experiments in the paper:

##### Random Insertions and Deletions

If you have downloaded the data via the ``downloader.py``, after you compile ``gfe_driver`` with the corresponding graph system, you can run the corresponding scripts to reproduce the results. For ``RadixGraph``, simply run:
```sh
sh scripts/run_random.sh radixgraph [threads]
```

Repeat the process by replacing ``radixgraph`` to ``stinger7-ref``, ``g1_v6-ref-ignore-build``, ``livegraph3_ro``, ``teseo.13``, ``sortledton.4``, ``bvgt`` and ``gtx``.

##### Sequential Insertions and Deletions

Repeat the same procedure with ``run_sequential.sh``:
```sh
sh scripts/run_sequential.sh radixgraph [threads]
```

##### Vertex Insertions Only

To generate vertex operations, compile:
```sh
g++ scripts/create_vertex_ops.cpp -o create_vertex_ops -O3
```

and run:
```sh
./create_vertex_ops
```

Then run the corresponding scripts to execute gfe_driver with the generated vertex operations:
```sh
sh scripts/run_vertex.sh radixgraph [threads]
```

Repeat the process by replacing ``radixgraph`` to ``stinger7-ref``, ``g1_v6-ref-ignore-build``, ``livegraph3_ro``, ``teseo.13``, ``sortledton.4``, ``bvgt`` and ``gtx``.

##### Memory Consumption

Reconfigure the graph systems with ``--enable-mem-analysis`` option. For example:
```sh
../configure --enable-optimize --enable-mem-analysis --disable-debug --with-radixgraph=../
```

Then run ``run_random.sh`` with the same procedure.

##### Mixed Updates

Generate the update log file with [graphlog](https://github.com/whatsthecraic/graphlog) and move them to the folder of ``gfe_driver``:
```sh
./graphlog -a 10 -e 1 -v 1 /path/to/gfe_driver/datasets/graph500-24.properties /path/to/gfe_driver/graph500-24-1.0.graphlog
./graphlog -a 10 -e 1 -v 1 /path/to/gfe_driver/datasets/uniform-24.properties /path/to/gfe_driver/uniform-24-1.0.graphlog
```

This will generate 10x operations of #edges of the original dataset, but the final built graph is still the same as the original graph.

Then execute:
```sh
sh scripts/run_mixed.sh radixgraph [threads]
```

Repeat the process by replacing ``radixgraph`` to ``stinger7-ref``, ``g1_v6-ref-ignore-build``, ``livegraph3_ro``, ``teseo.13``, ``sortledton.4``, ``bvgt`` and ``gtx``.

##### Graph Analytics

If you have downloaded the data via the ``downloader.py``, generate property files by:
```sh
g++ generate_property_files.cpp -o generate_property_files -O3
./generate_property_files datasets/twitter-2010.el
./generate_property_files datasets/com-lj.ungraph.el
./generate_property_files datasets/com-orkut.ungraph.el
```

After you compile ``gfe_driver`` with the corresponding graph system, you can run the corresponding scripts to reproduce the results. For ``RadixGraph``, simply run:
```sh
sh scripts/run_analytics.sh radixgraph [threads]
```

Repeat the process by replacing ``radixgraph`` to ``stinger7-ref``, ``g1_v6-ref-ignore-build``, ``livegraph3_ro``, ``teseo.13``, ``sortledton.4``, ``bvgt`` and ``gtx``.

##### Concurrent Reads and Writes

Generate the update log file with [graphlog](https://github.com/whatsthecraic/graphlog) for `dota-league`:
```sh
./graphlog -a 10 -e 1 -v 1 /path/to/gfe_driver/datasets/dota-league.properties /path/to/gfe_driver/dota-league.graphlog
```

Execute updates and 1-hop neighbor queries concurrently:
```sh
./build/gfe_driver -G datasets/dota-league.properties -u --log ./dota-league.graphlog --aging_timeout 48h -l radixgraph -r [num_read_threads] -w [num_write_threads] --mixed_workload true
```

Or simply by:
```sh
sh scripts/run_concurrent.sh radixgraph
```

which runs with with 4, 8, 16, 32 read/write threads.

To execute updates and 2-hop neighbor queries concurrently, firstly comment lines 164-171 in ``experiment/graphalytics.cpp`` and uncomment lines 174-182. Then execute the same command as above.

Repeat the process by replacing ``radixgraph`` to ``stinger7-ref``, ``g1_v6-ref-ignore-build``, ``livegraph3_ro``, ``teseo.13``, ``sortledton.4``, ``bvgt`` and ``gtx``.

##### Log compaction time for RadixGraph
Modify in ``library/radixgraph/RadixGraph/src/headers.h``:
```cpp
#define DEBUG_MODE true
```

and recompile. Then re-run the experiments. For example, if you want to measure the time for random insertions, simply re-run:
```
sh scripts/run_random.sh radixgraph
```

##### Plotting Figures

After executing above scripts, run the Python files in ``scripts`` folder like:
```sh
python3 scripts/plot_legends.py
python3 scripts/plot_updates.py
python3 scripts/plot_mixed.py
python3 scripts/plot_updates_vertices.py
python3 scripts/plot_memory.py
python3 scripts/plot_memory_vertex.py
python3 scripts/plot_analytics.py
```

The generated figures will be placed in the ``figures`` folder.

##### Summarize data

To collect data to CSV files, after you execute all experiments, run:
```sh
python3 scripts/data_to_csv.py
```

### Troubleshooting

As in requisites, we recommend using ``GCC 11.4.0`` and ``tbb 2022.01``. If you have installed multiple GCC and TBB versions, configure in your ``.bashrc`` file to ensure that the correct GCC and TBB are used:
```sh
alias gcc=gcc-11
alias g++=g++-11
export CC=gcc-11
export CXX=g++-11
export TBB_PATH=/path/to/tbb
export LD_LIBRARY_PATH=${TBB_PATH}/lib:$LD_LIBRARY_PATH
```

where ``/path/to/tbb`` is your installed TBB by compiling from source, which should contain ``include``, ``lib`` and ``share`` folders.

Following are some practical errors we met during configuration, most from (1) incompatible GCC compiler and libraries; (2) incompatible TBB versions.

For (1), you may get linking errors if you installed multiple versions of GCC. If you get errors like ``undefined reference to std::__throw_bad_array_new_length()``, try linking the standard C++ library with:
```sh
LDFLAGS += -L{LIBCPP_PATH} -lstdc++
```

in your ``Makefile`` before running ``make clean && make -j``, where ``LIBCPP_PATH`` is the path of ``libstdc++.so`` of GCC 10.5.0. Generally, it would be included in ``gcc/lib64`` of your manually installed GCC compiler. Also, remember to update your ``LD_LIBRARY_PATH`` by adding this to ``.bashrc``:
```sh
export LD_LIBRARY_PATH=${LIBCPP_PATH}:$LD_LIBRARY_PATH
```

For (2), you may encounter ``undefined reference to `tbb::detail::r1::throw_exception(tbb::detail::d0::exception_id)'`` when configuring LiveGraph, Spruce or GTX to GFE driver if your TBB version is incorrect (since they require different TBB versions), try installing the correct version of the TBB (2020.3-1 for LiveGraph and 2022.01 for others) by compiling from source and linking the TBB library by adding following to ``build/Makefile``:
```sh
LDFLAGS += -I${TBB_PATH}/include -L${TBB_PATH}/lib -ltbb
```

When compiling the library of GTX, you may also encounter ``error: invalid ‘static_cast’ from type ‘const std::thread::id’ to type ‘std::size_t’ {aka ‘long unsigned int’}`` if your TBB version is incorrect. Similarly, try installing the correct version of the TBB and add TBB path to ``CMakeLists.txt`` in GTX:
```sh
set(CMAKE_PREFIX_PATH "/path/to/tbb")
```

We understand that the driver is difficult to configure and you may encounter some issues. In fact, we have tried to simplify the process of executing the driver compared with the original repo. Feel free to ask us for help and provide suggestions.