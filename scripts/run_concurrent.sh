if [ ! -d "results" ]; then
    mkdir results
fi
if [ -n "$1" ]; then
    if [ ! -d "results/$1" ]; then
        mkdir "results/$1"
    fi
    if [ ! -d "results/$1/concurrent" ]; then
        mkdir "results/$1/concurrent"
    fi
    if [ "$1" = "radixgraph" ]; then
        ./optimizer datasets/dota-league.v 8 > settings
    fi
    for a in 4 8 16 32
    do
        ./build/gfe_driver -G datasets/dota-league.properties -u --log ./dota-league.graphlog --aging_timeout 48h -l $1 -r $a -w 32 --mixed_workload true > results/$1/concurrent/dota-league-$a-read-threads-$1
        ./build/gfe_driver -G datasets/dota-league.properties -u --log ./dota-league.graphlog --aging_timeout 48h -l $1 -r 32 -w $a --mixed_workload true > results/$1/concurrent/dota-league-$a-write-threads-$1
    done
    if [ "$1" = "radixgraph" ]; then
        ./optimizer datasets/graph500-24.v 8 > settings
    fi
    for a in 4 8 16 32
    do
        ./build/gfe_driver -G datasets/graph500-24.properties -u --log ./graph500-24-1.0.graphlog --aging_timeout 48h -l $1 -r $a -w 32 --mixed_workload true > results/$1/concurrent/graph500-24-$a-read-threads-$
        ./build/gfe_driver -G datasets/graph500-24.properties -u --log ./graph500-24-1.0.graphlog --aging_timeout 48h -l $1 -r 32 -w $a --mixed_workload true > results/$1/concurrent/graph500-24-$a-write-threads-$1
    done
    if [ "$1" = "radixgraph" ]; then
        rm settings
    fi
fi