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
        if [ "$1" = "gtx" ]; then # If your machine memory is large enough, comment this
            mv graph500-24-1.0.graphlog graph500-24-1.0.graphlog-original
            head -n $(($(wc -l < graph500-24-1.0.graphlog-original) * 20 / 100)) graph500-24-1.0.graphlog-original > graph500-24-1.0.graphlog
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u --log ./graph500-24-1.0.graphlog --aging_timeout 48h -l $1 -w $2 --aging_memfp --aging_memfp_physical -d results/$1/mixed/graph500-24-mixed-$1.sqlite3 > results/$1/mixed/graph500-24-mixed-$1
        if [ "$1" = "gtx" ]; then # If your machine memory is large enough, comment this
            mv graph500-24-1.0.graphlog-original graph500-24-1.0.graphlog
        fi
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/uniform-24.v 8 > settings
        fi
        if [ "$1" = "gtx" ]; then # If your machine memory is large enough, comment this
            mv uniform-24-1.0.graphlog uniform-24-1.0.graphlog-original
            head -n $(($(wc -l < uniform-24-1.0.graphlog-original) * 20 / 100)) uniform-24-1.0.graphlog-original > uniform-24-1.0.graphlog
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u --log ./uniform-24-1.0.graphlog --aging_timeout 48h -l $1 -w $2 --aging_memfp --aging_memfp_physical -d results/$1/mixed/uniform-24-mixed-$1.sqlite3 > results/$1/mixed/uniform-24-mixed-$1
        if [ "$1" = "gtx" ]; then # If your machine memory is large enough, comment this
            mv uniform-24-1.0.graphlog-original uniform-24-1.0.graphlog
        fi
    else
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/graph500-24.v 8 > settings
        fi
        if [ "$1" = "gtx" ]; then # If your machine memory is large enough, comment this
            mv graph500-24-1.0.graphlog graph500-24-1.0.graphlog-original
            head -n $(($(wc -l < graph500-24-1.0.graphlog-original) * 20 / 100)) graph500-24-1.0.graphlog-original > graph500-24-1.0.graphlog
        fi
        ./build/gfe_driver -G datasets/graph500-24.properties -u --log ./graph500-24-1.0.graphlog --aging_timeout 48h -l $1 -w $(nproc) --aging_memfp --aging_memfp_physical -d results/$1/mixed/graph500-24-mixed-$1.sqlite3 > results/$1/mixed/graph500-24-mixed-$1
        if [ "$1" = "gtx" ]; then # If your machine memory is large enough, comment this
            mv graph500-24-1.0.graphlog-original graph500-24-1.0.graphlog
        fi
        if [ "$1" = "radixgraph" ]; then
            ./optimizer datasets/uniform-24.v 8 > settings
        fi
        if [ "$1" = "gtx" ]; then # If your machine memory is large enough, comment this
            mv uniform-24-1.0.graphlog uniform-24-1.0.graphlog-original
            head -n $(($(wc -l < uniform-24-1.0.graphlog-original) * 20 / 100)) uniform-24-1.0.graphlog-original > uniform-24-1.0.graphlog
        fi
        ./build/gfe_driver -G datasets/uniform-24.properties -u --log ./uniform-24-1.0.graphlog --aging_timeout 48h -l $1 -w $(nproc) --aging_memfp --aging_memfp_physical -d results/$1/mixed/uniform-24-mixed-$1.sqlite3 > results/$1/mixed/uniform-24-mixed-$1
        if [ "$1" = "gtx" ]; then # If your machine memory is large enough, comment this
            mv uniform-24-1.0.graphlog-original uniform-24-1.0.graphlog
        fi
    fi
    if [ "$1" = "radixgraph" ]; then
        rm settings
    fi
fi