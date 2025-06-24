if [ ! -d "results" ]; then
    mkdir results
fi
if [ -n "$1" ]; then
    if [ ! -d "results/$1" ]; then
        mkdir "results/$1"
    fi
    if [ ! -d "results/$1/random" ]; then
        mkdir "results/$1/random"
    fi
    if [ -n "$2" ]; then
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/graph500-24.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $2 > results/$1/random/graph500-24-random-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/graph500-26.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/graph500-26.properties -u -l $1 -w $2 > results/$1/random/graph500-26-random-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/uniform-24.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $2 > results/$1/random/uniform-24-random-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/uniform-26.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/uniform-26.properties -u -l $1 -w $2 > results/$1/random/uniform-26-random-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/dota-league.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $2 > results/$1/random/dota-league-random-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/com-lj.ungraph.el 8 > settings
        fi
        ./build/gfe_driver -G datasets/com-lj.ungraph.el -u -l $1 -w $2 > results/$1/random/com-lj.ungraph-random-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/com-orkut.ungraph.el 8 > settings
        fi
        ./build/gfe_driver -G datasets/com-orkut.ungraph.el -u -l $1 -w $2 > results/$1/random/com-orkut.ungraph-random-$1
    else
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/graph500-24.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $(nproc) > results/$1/random/graph500-24-random-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/graph500-26.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/graph500-26.properties -u -l $1 -w $(nproc) > results/$1/random/graph500-26-random-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/uniform-24.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $(nproc) > results/$1/random/uniform-24-random-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/uniform-26.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/uniform-26.properties -u -l $1 -w $(nproc) > results/$1/random/uniform-26-random-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/dota-league.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $(nproc) > results/$1/random/dota-league-random-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/com-lj.ungraph.el 8 > settings
        fi
        ./build/gfe_driver -G datasets/com-lj.ungraph.el -u -l $1 -w $(nproc) > results/$1/random/com-lj.ungraph-random-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/com-orkut.ungraph.el 8 > settings
        fi
        ./build/gfe_driver -G datasets/com-orkut.ungraph.el -u -l $1 -w $(nproc) > results/$1/random/com-orkut.ungraph-random-$1
    fi
    if [ "$1" = "radixgraph" ]; then
        rm settings
    fi
fi