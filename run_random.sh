if [ ! -d "results" ]; then
    mkdir results
fi
if [ -n "$1" ]; then
    ./optimizer datasets/graph500-24.v 8 > settings
    ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w 16 > results/graph500-24-$1
    ./optimizer datasets/graph500-26.v 8 > settings
    ./build/gfe_driver -G datasets/graph500-26.properties -u -l $1 -w 16 > results/graph500-26-$1
    ./optimizer datasets/uniform-24.v 8 > settings
    ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w 16 > results/uniform-24-$1
    ./optimizer datasets/uniform-26.v 8 > settings
    ./build/gfe_driver -G datasets/uniform-26.properties -u -l $1 -w 16 > results/uniform-26-$1
    ./optimizer datasets/yahoo-song.el 8 > settings
    ./build/gfe_driver -G datasets/yahoo-song.el -u -l $1 -w 16 > results/uniform-26-$1
    rm settings
fi