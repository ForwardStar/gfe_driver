if [ ! -d "results" ]; then
    mkdir results
fi
if [ -n "$1" ]; then
    if [ ! -d "results/$1" ]; then
        mkdir "results/$1"
    fi
    if [ ! -d "results/$1/mixed" ]; then
        mkdir "results/$1/mixed"
    fi
    if [ -n "$2" ]; then
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/graph500-24.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u --log ./graph500-24-1.0.graphlog --aging_timeout 48h -l $1 -w $2 --aging_memfp_physical > results/$1/mixed/graph500-24-mixed-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/uniform-24.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u --log ./uniform-24-1.0.graphlog --aging_timeout 48h -l $1 -w $2 --aging_memfp_physical > results/$1/mixed/uniform-24-mixed-$1
    else
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/graph500-24.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u --log ./graph500-24-1.0.graphlog --aging_timeout 48h -l $1 -w $(nproc) --aging_memfp_physical > results/$1/mixed/graph500-24-mixed-$1
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/uniform-24.v 8 > settings
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u --log ./uniform-24-1.0.graphlog --aging_timeout 48h -l $1 -w $(nproc) --aging_memfp_physical > results/$1/mixed/uniform-24-mixed-$1
    fi
    if [ "$1" = "radixgraph" ]; then
        rm settings
    fi
fi