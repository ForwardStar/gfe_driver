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
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/graph500-24.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $2 > results/$1/sequential/graph500-24-sequential-$1 --is_timestamped true
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/graph500-26.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/graph500-26.properties -u -l $1 -w $2 > results/$1/sequential/graph500-26-sequential-$1 --is_timestamped true
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/uniform-24.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $2 > results/$1/sequential/uniform-24-sequential-$1 --is_timestamped true
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/uniform-26.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/uniform-26.properties -u -l $1 -w $2 > results/$1/sequential/uniform-26-sequential-$1 --is_timestamped true
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/dota-league.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $2 > results/$1/sequential/dota-league-sequential-$1 --is_timestamped true
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-friendster.ungraph.el 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/com-friendster.ungraph.el -u -l $1 -w $2 > results/$1/sequential/com-friendster.ungraph-sequential-$1 --is_timestamped true
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-lj.ungraph.el 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/com-lj.ungraph.el -u -l $1 -w $2 > results/$1/sequential/com-lj.ungraph-sequential-$1 --is_timestamped true
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-orkut.ungraph.el 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/com-orkut.ungraph.el -u -l $1 -w $2 > results/$1/sequential/com-orkut.ungraph-sequential-$1 --is_timestamped true
    else
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/graph500-24.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $(nproc) > results/$1/sequential/graph500-24-sequential-$1 --is_timestamped true
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/graph500-26.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/graph500-26.properties -u -l $1 -w $(nproc) > results/$1/sequential/graph500-26-sequential-$1 --is_timestamped true
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/uniform-24.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $(nproc) > results/$1/sequential/uniform-24-sequential-$1 --is_timestamped true
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/uniform-26.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/uniform-26.properties -u -l $1 -w $(nproc) > results/$1/sequential/uniform-26-sequential-$1 --is_timestamped true
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/dota-league.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $(nproc) > results/$1/sequential/dota-league-sequential-$1 --is_timestamped true
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-friendster.ungraph.el 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/com-friendster.ungraph.el -u -l $1 -w $(nproc) > results/$1/sequential/com-friendster.ungraph-sequential-$1 --is_timestamped true
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-lj.ungraph.el 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/com-lj.ungraph.el -u -l $1 -w $(nproc) > results/$1/sequential/com-lj.ungraph-sequential-$1 --is_timestamped true
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-orkut.ungraph.el 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/com-orkut.ungraph.el -u -l $1 -w $(nproc) > results/$1/sequential/com-orkut.ungraph-sequential-$1 --is_timestamped true
    fi
    if [ "$1" = "forward_star" ]; then
        rm settings
    fi
fi