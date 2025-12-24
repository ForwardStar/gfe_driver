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
    if [ -n "$3" ]; then
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/graph500-24.v 8 
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $2 -r $2 -R $3 --blacklist cdlp > results/$1/analytics/graph500-24-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/uniform-24.v 8 
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $2 -r $2 -R $3 --blacklist cdlp > results/$1/analytics/uniform-24-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/dota-league.v 8 
        fi
        ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $2 -r $2 -R $3 --blacklist cdlp > results/$1/analytics/dota-league-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/com-orkut.ungraph.v 8 
        fi
        ./build/gfe_driver -G datasets/com-orkut.ungraph.properties -u -l $1 -w $2 -r $2 -R $3 --blacklist cdlp > results/$1/analytics/com-orkut-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/com-lj.ungraph.v 8 
        fi
        ./build/gfe_driver -G datasets/com-lj.ungraph.properties -u -l $1 -w $2 -r $2 -R $3 --blacklist cdlp > results/$1/analytics/com-lj-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/twitter-2010.v 8 
        fi
        ./build/gfe_driver -G datasets/twitter-2010.properties -u -l $1 -w $2 -r $2 -R $3 --blacklist cdlp > results/$1/analytics/twitter-2010-analytics-$1
    elif [ -n "$2" ]; then
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/graph500-24.v 8 
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $2 -r $2 -R 5 --blacklist cdlp > results/$1/analytics/graph500-24-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/uniform-24.v 8 
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $2 -r $2 -R 5 --blacklist cdlp > results/$1/analytics/uniform-24-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/dota-league.v 8 
        fi
        ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $2 -r $2 -R 5 --blacklist cdlp > results/$1/analytics/dota-league-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/com-orkut.ungraph.v 8 
        fi
        ./build/gfe_driver -G datasets/com-orkut.ungraph.properties -u -l $1 -w $2 -r $2 -R 5 --blacklist cdlp > results/$1/analytics/com-orkut-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/com-lj.ungraph.v 8 
        fi
        ./build/gfe_driver -G datasets/com-lj.ungraph.properties -u -l $1 -w $2 -r $2 -R 5 --blacklist cdlp > results/$1/analytics/com-lj-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/twitter-2010.v 8 
        fi
        ./build/gfe_driver -G datasets/twitter-2010.properties -u -l $1 -w $2 -r $2 -R 5 --blacklist cdlp > results/$1/analytics/twitter-2010-analytics-$1
    else
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/graph500-24.v 8 
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u -l $1 -w $(nproc) -r $(nproc) -R 5 --blacklist cdlp > results/$1/analytics/graph500-24-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/uniform-24.v 8 
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u -l $1 -w $(nproc) -r $(nproc) -R 5 --blacklist cdlp > results/$1/analytics/uniform-24-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/dota-league.v 8 
        fi
        ./build/gfe_driver -G datasets/dota-league.properties -u -l $1 -w $(nproc) -r $(nproc) -R 5 --blacklist cdlp > results/$1/analytics/dota-league-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/com-orkut.ungraph.v 8 
        fi
        ./build/gfe_driver -G datasets/com-orkut.ungraph.properties -u -l $1 -w $(nproc) -r $(nproc) -R 5 --blacklist cdlp > results/$1/analytics/com-orkut-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/com-lj.ungraph.v 8 
        fi
        ./build/gfe_driver -G datasets/com-lj.ungraph.properties -u -l $1 -w $(nproc) -r $(nproc) -R 5 --blacklist cdlp > results/$1/analytics/com-lj-analytics-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/twitter-2010.v 8 
        fi
        ./build/gfe_driver -G datasets/twitter-2010.properties -u -l $1 -w $(nproc) -r $(nproc) -R 5 --blacklist cdlp > results/$1/analytics/twitter-2010-analytics-$1
    fi
    if [ "$1" = "radixgraph" ]; then
        rm settings.txt
    fi
fi