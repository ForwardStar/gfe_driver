if [ ! -d "results" ]; then
    mkdir results
fi
if [ -n "$1" ]; then
    if [ ! -d "results/$1" ]; then
        mkdir "results/$1"
    fi
    if [ ! -d "results/$1/analytics" ]; then
        mkdir "results/$1/analytics"
    fi
    if [ -n "$2" ]; then
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/graph500-24.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $2 -r $2 -R 5 --blacklist cdlp > results/$1/analytics/graph500-24-analytics-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/graph500-26.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/graph500-26.properties -u -l $1 -w $2 -r $2 -R 5 --blacklist cdlp > results/$1/analytics/graph500-26-analytics-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/uniform-24.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $2 -r $2 -R 5 --blacklist cdlp > results/$1/analytics/uniform-24-analytics-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/uniform-26.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/uniform-26.properties -u -l $1 -w $2 -r $2 -R 5 --blacklist cdlp > results/$1/analytics/uniform-26-analytics-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/dota-league.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $2 -r $2 -R 5 --blacklist cdlp > results/$1/analytics/dota-league-analytics-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-friendster.ungraph.el 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/com-friendster.ungraph.el -u -l $1 -w $2 -r $2 -R 5 --blacklist cdlp > results/$1/analytics/com-friendster.ungraph-analytics-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-lj.ungraph.el 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/com-lj.ungraph.el -u -l $1 -w $2 -r $2 -R 5 --blacklist cdlp > results/$1/analytics/com-lj.ungraph-analytics-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-orkut.ungraph.el 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/com-orkut.ungraph.el -u -l $1 -w $2 -r $2 -R 5 --blacklist cdlp > results/$1/analytics/com-orkut.ungraph-analytics-$1
    else
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/graph500-24.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $(nproc) -r $2 -R 5 --blacklist cdlp > results/$1/analytics/graph500-24-analytics-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/graph500-26.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/graph500-26.properties -u -l $1 -w $(nproc) -r $2 -R 5 --blacklist cdlp > results/$1/analytics/graph500-26-analytics-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/uniform-24.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $(nproc) -r $2 -R 5 --blacklist cdlp > results/$1/analytics/uniform-24-analytics-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/uniform-26.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/uniform-26.properties -u -l $1 -w $(nproc) -r $2 -R 5 --blacklist cdlp > results/$1/analytics/uniform-26-analytics-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/dota-league.v 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $(nproc) -r $2 -R 5 --blacklist cdlp > results/$1/analytics/dota-league-analytics-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-friendster.ungraph.el 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/com-friendster.ungraph.el -u -l $1 -w $(nproc) -r $2 -R 5 --blacklist cdlp > results/$1/analytics/com-friendster.ungraph-analytics-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-lj.ungraph.el 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/com-lj.ungraph.el -u -l $1 -w $(nproc) -r $2 -R 5 --blacklist cdlp > results/$1/analytics/com-lj.ungraph-analytics-$1
        if [ "$1" = "forward_star" ]; then
            ./optimizer datasets/com-orkut.ungraph.el 8 > settings
            echo "0" >> settings
        fi
        ./build/gfe_driver -G datasets/com-orkut.ungraph.el -u -l $1 -w $(nproc) -r $2 -R 5 --blacklist cdlp > results/$1/analytics/com-orkut.ungraph-analytics-$1
    fi
    if [ "$1" = "forward_star" ]; then
        rm settings
    fi
fi