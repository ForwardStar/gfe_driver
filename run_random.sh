if [ ! -d "results" ]; then
    mkdir results
fi
if [ -n "$1" ]; then
    if [ -n "$2" ]; then
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/graph500-24.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $2 > results/graph500-24-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/graph500-26.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/graph500-26.properties -u -l $1 -w $2 > results/graph500-26-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/uniform-24.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $2 > results/uniform-24-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/uniform-26.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/uniform-26.properties -u -l $1 -w $2 > results/uniform-26-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/dota-league.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $2 > results/dota-league-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/yahoo-song.el 8 > settings
        fi
        ./build/gfe_driver -G datasets/yahoo-song.el -u -l $1 -w $2 > results/yahoo-song-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-friendster.ungraph.el 8 > settings
        fi
        ./build/gfe_driver -G datasets/com-friendster.ungraph.el -u -l $1 -w $2 > results/com-friendster.ungraph-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-lj.ungraph.el 8 > settings
        fi
        ./build/gfe_driver -G datasets/com-lj.ungraph.el -u -l $1 -w $2 > results/com-lj.ungraph-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-orkut.ungraph.el 8 > settings
        fi
        ./build/gfe_driver -G datasets/com-orkut.ungraph.el -u -l $1 -w $2 > results/com-orkut.ungraph-random-$1
    else
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/graph500-24.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $(nproc) > results/graph500-24-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/graph500-26.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/graph500-26.properties -u -l $1 -w $(nproc) > results/graph500-26-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/uniform-24.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $(nproc) > results/uniform-24-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/uniform-26.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/uniform-26.properties -u -l $1 -w $(nproc) > results/uniform-26-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/dota-league.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $(nproc) > results/dota-league-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/yahoo-song.el 8 > settings
        fi
        ./build/gfe_driver -G datasets/yahoo-song.el -u -l $1 -w $(nproc) > results/yahoo-song-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-friendster.ungraph.el 8 > settings
        fi
        ./build/gfe_driver -G datasets/com-friendster.ungraph.el -u -l $1 -w $(nproc) > results/com-friendster.ungraph-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-lj.ungraph.el 8 > settings
        fi
        ./build/gfe_driver -G datasets/com-lj.ungraph.el -u -l $1 -w $(nproc) > results/com-lj.ungraph-random-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-orkut.ungraph.el 8 > settings
        fi
        ./build/gfe_driver -G datasets/com-orkut.ungraph.el -u -l $1 -w $(nproc) > results/com-orkut.ungraph-random-$1
    fi
    if [ "$1" = "forward_star" ]; then
        rm settings
    fi
fi