if [ ! -d "results" ]; then
    mkdir results
fi
if [ -n "$1" ]; then
    if [ ! -d "results/$1" ]; then
        mkdir "results/$1"
    fi
    if [ ! -d "results/$1/sequential" ]; then
        mkdir "results/$1/sequential"
    fi
    if [ -n "$2" ]; then
        ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $2 --is_timestamped true > results/$1/sequential/graph500-24-sequential-$1
        ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $2 --is_timestamped true > results/$1/sequential/uniform-24-sequential-$1
        ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $2 --is_timestamped true > results/$1/sequential/dota-league-sequential-$1
        ./build/gfe_driver -G datasets/com-lj.ungraph.el -u -l $1 -w $2 --is_timestamped true > results/$1/sequential/com-lj.ungraph-sequential-$1
        ./build/gfe_driver -G datasets/com-orkut.ungraph.el -u -l $1 -w $2 --is_timestamped true > results/$1/sequential/com-orkut.ungraph-sequential-$1
        ./build/gfe_driver -G datasets/twitter-2010.el -u -l $1 -w $2 --is_timestamped true > results/$1/sequential/twitter-2010-sequential-$1
    else
        ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $(nproc) --is_timestamped true > results/$1/sequential/graph500-24-sequential-$1
        ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $(nproc) --is_timestamped true > results/$1/sequential/uniform-24-sequential-$1
        ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $(nproc) --is_timestamped true > results/$1/sequential/dota-league-sequential-$1
        ./build/gfe_driver -G datasets/com-lj.ungraph.el -u -l $1 -w $(nproc) --is_timestamped true > results/$1/sequential/com-lj.ungraph-sequential-$1
        ./build/gfe_driver -G datasets/com-orkut.ungraph.el -u -l $1 -w $(nproc) --is_timestamped true > results/$1/sequential/com-orkut.ungraph-sequential-$1
        ./build/gfe_driver -G datasets/twitter-2010.el -u -l $1 -w $(nproc) --is_timestamped true > results/$1/sequential/twitter-2010-sequential-$1
    fi
fi