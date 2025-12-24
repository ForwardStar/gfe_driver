if [ ! -d "results" ]; then
    mkdir results
fi
if [ -n "$1" ]; then
    if [ ! -d "results/$1" ]; then
        mkdir "results/$1"
    fi
    if [ ! -d "results/$1/delete-only" ]; then
        mkdir "results/$1/delete-only"
    fi
    if [ -n "$2" ]; then
        ./build/gfe_driver -G datasets/graph500-24.properties -u --log ./graph500-24-delete.graphlog --aging_timeout 48h -l $1 -w $2 --aging_memfp --aging_memfp_physical --aging_release_memory false --delete_all true > results/$1/delete-only/graph500-24-mixed-$1
        ./build/gfe_driver -G datasets/uniform-24.properties -u --log ./uniform-24-delete.graphlog --aging_timeout 48h -l $1 -w $2 --aging_memfp --aging_memfp_physical --aging_release_memory false --delete_all true > results/$1/delete-only/uniform-24-mixed-$1
    else
        ./build/gfe_driver -G datasets/graph500-24.properties -u --log ./graph500-24-delete.graphlog -l $1 -w $(nproc) --aging_timeout 48h --aging_memfp --aging_memfp_physical --aging_release_memory false --delete_all true > results/$1/delete-only/graph500-24-mixed-$1
        ./build/gfe_driver -G datasets/uniform-24.properties -u --log ./uniform-24-delete.graphlog -l $1 -w $(nproc) --aging_timeout 48h --aging_memfp --aging_memfp_physical --aging_release_memory false --delete_all true > results/$1/delete-only/uniform-24-mixed-$1
    fi
fi