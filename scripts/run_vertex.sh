if [ ! -d "results" ]; then
    mkdir results
fi
if [ -n "$1" ]; then
    if [ ! -d "results/$1" ]; then
        mkdir "results/$1"
    fi
    if [ ! -d "results/$1/vertices" ]; then
        mkdir "results/$1/vertices"
    fi
    if [ -n "$2" ]; then
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/graph500-24.v 8 
        fi
        ./build/gfe_driver -G datasets/graph500-24.vertices.el -u -l $1 -w $2 --insert_vertex_only true > results/$1/vertices/graph500-24-vertices-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/uniform-24.v 8 
        fi
        ./build/gfe_driver -G datasets/uniform-24.vertices.el -u -l $1 -w $2 --insert_vertex_only true > results/$1/vertices/uniform-24-vertices-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/dota-league.v 8 
        fi
        ./build/gfe_driver -G datasets/dota-league.vertices.el -u -l $1 -w $2 --insert_vertex_only true > results/$1/vertices/dota-league-vertices-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/com-lj.ungraph.vertices.el 8 
        fi
        ./build/gfe_driver -G datasets/com-lj.ungraph.vertices.el -u -l $1 -w $2 --insert_vertex_only true > results/$1/vertices/com-lj.ungraph-vertices-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/com-orkut.ungraph.vertices.el 8 
        fi
        ./build/gfe_driver -G datasets/com-orkut.ungraph.vertices.el -u -l $1 -w $2 --insert_vertex_only true > results/$1/vertices/com-orkut.ungraph-vertices-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/twitter-2010.vertices.el 8 
        fi
        ./build/gfe_driver -G datasets/twitter-2010.vertices.el -u -l $1 -w $2 --insert_vertex_only true > results/$1/vertices/twitter-2010-vertices-$1
    else
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/graph500-24.v 8 
        fi
        ./build/gfe_driver -G datasets/graph500-24.vertices.el -u -l $1 -w $(nproc) --insert_vertex_only true > results/$1/vertices/graph500-24-vertices-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/uniform-24.v 8 
        fi
        ./build/gfe_driver -G datasets/uniform-24.vertices.el -u -l $1 -w $(nproc) --insert_vertex_only true > results/$1/vertices/uniform-24-vertices-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/dota-league.v 8 
        fi
        ./build/gfe_driver -G datasets/dota-league.vertices.el -u -l $1 -w $(nproc) --insert_vertex_only true > results/$1/vertices/dota-league-vertices-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/com-lj.ungraph.vertices.el 8 
        fi
        ./build/gfe_driver -G datasets/com-lj.ungraph.vertices.el -u -l $1 -w $(nproc) --insert_vertex_only true > results/$1/vertices/com-lj.ungraph-vertices-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/com-orkut.ungraph.vertices.el 8 
        fi
        ./build/gfe_driver -G datasets/com-orkut.ungraph.vertices.el -u -l $1 -w $(nproc) --insert_vertex_only true > results/$1/vertices/com-orkut.ungraph-vertices-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/twitter-2010.vertices.el 8 
        fi
        ./build/gfe_driver -G datasets/twitter-2010.vertices.el -u -l $1 -w $(nproc) --insert_vertex_only true > results/$1/vertices/twitter-2010-vertices-$1
    fi
    if [ "$1" = "radixgraph" ]; then
        rm settings.txt
    fi
fi