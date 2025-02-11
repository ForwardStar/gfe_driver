if [ ! -d "results" ]; then
    mkdir results
fi
if [ -n "$1" ]; then
    ./optimizer datasets/graph500-24.v 8 > settings
    ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w 16 > results/graph500-24-random-$1
    ./optimizer datasets/graph500-26.v 8 > settings
    ./build/gfe_driver -G datasets/graph500-26.properties -u -l $1 -w 16 > results/graph500-26-random-$1
    ./optimizer datasets/uniform-24.v 8 > settings
    ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w 16 > results/uniform-24-random-$1
    ./optimizer datasets/uniform-26.v 8 > settings
    ./build/gfe_driver -G datasets/uniform-26.properties -u -l $1 -w 16 > results/uniform-26-random-$1
    ./optimizer datasets/dota-league.v 8 > settings
    ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w 16 > results/dota-league-random-$1
    ./optimizer datasets/yahoo-song.el 8 > settings
    ./build/gfe_driver -G datasets/yahoo-song.el -u -l $1 -w 16 > results/yahoo-song-random-$1
    ./optimizer datasets/com-friendster.ungraph.txt 8 > settings
    ./build/gfe_driver -G datasets/com-friendster.ungraph.txt -u -l $1 -w 16 > results/com-friendster.ungraph-random-$1
    ./optimizer datasets/com-lj.ungraph.txt 8 > settings
    ./build/gfe_driver -G datasets/com-lj.ungraph.txt -u -l $1 -w 16 > results/com-lj.ungraph-random-$1
    ./optimizer datasets/com-orkut.ungraph.txt 8 > settings
    ./build/gfe_driver -G datasets/com-orkut.ungraph.txt -u -l $1 -w 16 > results/com-orkut.ungraph-random-$1
    rm settings
fi