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
        if [ -e "datasets/graph500-24.properties" ]; then
            if [ "$1" = "radixgraph" ]; then
                ./optimizer datasets/graph500-24.v 8 
            fi
            ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $2 > results/$1/random/graph500-24-random-$1
        fi
        if [ -e "datasets/uniform-24.properties" ]; then
            if [ "$1" = "radixgraph" ]; then
                ./optimizer datasets/uniform-24.v 8 
            fi
            ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $2 > results/$1/random/uniform-24-random-$1
        fi
        if [ -e "datasets/dota-league.properties" ]; then
            if [ "$1" = "radixgraph" ]; then
                ./optimizer datasets/dota-league.v 8 
            fi
            ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $2 > results/$1/random/dota-league-random-$1
        fi
        if [ -e "datasets/com-lj.ungraph.el" ]; then
            if [ "$1" = "radixgraph" ]; then
                ./optimizer datasets/com-lj.ungraph.el 8 
            fi
            ./build/gfe_driver -G datasets/com-lj.ungraph.el -u -l $1 -w $2 > results/$1/random/com-lj.ungraph-random-$1
        fi
        if [ -e "datasets/com-orkut.ungraph.el" ]; then
            if [ "$1" = "radixgraph" ]; then
                ./optimizer datasets/com-orkut.ungraph.el 8 
            fi
            ./build/gfe_driver -G datasets/com-orkut.ungraph.el -u -l $1 -w $2 > results/$1/random/com-orkut.ungraph-random-$1
        fi
        if [ -e "datasets/twitter-2010.el" ]; then
            if [ "$1" = "radixgraph" ]; then
                ./optimizer datasets/twitter-2010.el 8 
            fi
            ./build/gfe_driver -G datasets/twitter-2010.el -u -l $1 -w $2 > results/$1/random/twitter-2010-random-$1
        fi
    else
        if [ -e "datasets/graph500-24.properties" ]; then
            if [ "$1" = "radixgraph" ]; then
                ./optimizer datasets/graph500-24.v 8 
            fi
            ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $(nproc) > results/$1/random/graph500-24-random-$1
        fi
        if [ -e "datasets/uniform-24.properties" ]; then
            if [ "$1" = "radixgraph" ]; then
                ./optimizer datasets/uniform-24.v 8 
            fi
            ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $(nproc) > results/$1/random/uniform-24-random-$1
        fi
        if [ -e "datasets/dota-league.properties" ]; then
            if [ "$1" = "radixgraph" ]; then
                ./optimizer datasets/dota-league.v 8 
            fi
            ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $(nproc) > results/$1/random/dota-league-random-$1
        fi
        if [ -e "datasets/com-lj.ungraph.el" ]; then
            if [ "$1" = "radixgraph" ]; then
                ./optimizer datasets/com-lj.ungraph.el 8 
            fi
            ./build/gfe_driver -G datasets/com-lj.ungraph.el -u -l $1 -w $(nproc) > results/$1/random/com-lj.ungraph-random-$1
        fi
        if [ -e "datasets/com-orkut.ungraph.el" ]; then
            if [ "$1" = "radixgraph" ]; then
                ./optimizer datasets/com-orkut.ungraph.el 8 
            fi
            ./build/gfe_driver -G datasets/com-orkut.ungraph.el -u -l $1 -w $(nproc) > results/$1/random/com-orkut.ungraph-random-$1
        fi
        if [ -e "datasets/twitter-2010.el" ]; then
            if [ "$1" = "radixgraph" ]; then
                ./optimizer datasets/twitter-2010.el 8 
            fi
            ./build/gfe_driver -G datasets/twitter-2010.el -u -l $1 -w $(nproc) > results/$1/random/twitter-2010-random-$1
        fi
    fi
    if [ "$1" = "radixgraph" ]; then
        if [ -e "settings.txt" ]; then
            rm settings.txt
        fi
    fi
fi