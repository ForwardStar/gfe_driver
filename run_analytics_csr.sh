if [ ! -d "results" ]; then
    mkdir results
fi
if [ ! -d "results/csr3" ]; then
    mkdir "results/csr3"
fi
if [ ! -d "results/csr3/analytics" ]; then
    mkdir "results/csr3/analytics"
fi
if [ -n "$1" ]; then
    ./build/gfe_driver -G datasets/graph500-24.properties -u -l csr3 --load -r $1 -R 5 --blacklist lcc,cdlp > results/csr3/analytics/graph500-24-analytics-csr3
    ./build/gfe_driver -G datasets/graph500-26.properties -u -l csr3 --load -r $1 -R 5 --blacklist lcc,cdlp > results/csr3/analytics/graph500-26-analytics-csr3
    ./build/gfe_driver -G datasets/uniform-24.properties -u -l csr3 --load -r $1 -R 5 --blacklist lcc,cdlp > results/csr3/analytics/uniform-24-analytics-csr3
    ./build/gfe_driver -G datasets/uniform-26.properties -u -l csr3 --load -r $1 -R 5 --blacklist lcc,cdlp > results/csr3/analytics/uniform-26-analytics-csr3
    ./build/gfe_driver -G datasets/dota-league.properties -u -l csr3 --load -r $1 -R 5 --blacklist lcc,cdlp > results/csr3/analytics/dota-league-analytics-csr3
    ./build/gfe_driver -G datasets/com-friendster.ungraph.el -u -l csr3 --load -r $1 -R 5 --blacklist lcc,cdlp > results/csr3/analytics/com-friendster.ungraph-analytics-csr3
    ./build/gfe_driver -G datasets/com-lj.ungraph.el -u -l csr3 --load -r $1 -R 5 --blacklist lcc,cdlp > results/csr3/analytics/com-lj.ungraph-analytics-csr3
    ./build/gfe_driver -G datasets/com-orkut.ungraph.el -u -l csr3 --load -r $1 -R 5 --blacklist lcc,cdlp > results/csr3/analytics/com-orkut.ungraph-analytics-csr3
else
    ./build/gfe_driver -G datasets/graph500-24.properties -u -l csr3 --load -w $(nproc) -r $(nproc) -R 5 --blacklist lcc,cdlp > results/csr3/analytics/graph500-24-analytics-csr3
    ./build/gfe_driver -G datasets/graph500-26.properties -u -l csr3 --load -w $(nproc) -r $(nproc) -R 5 --blacklist lcc,cdlp > results/csr3/analytics/graph500-26-analytics-csr3
    ./build/gfe_driver -G datasets/uniform-24.properties -u -l csr3 --load -w $(nproc) -r $(nproc) -R 5 --blacklist lcc,cdlp > results/csr3/analytics/uniform-24-analytics-csr3
    ./build/gfe_driver -G datasets/uniform-26.properties -u -l csr3 --load -w $(nproc) -r $(nproc) -R 5 --blacklist lcc,cdlp > results/csr3/analytics/uniform-26-analytics-csr3
    ./build/gfe_driver -G datasets/dota-league.properties -u -l csr3 --load -w $(nproc) -r $(nproc) -R 5 --blacklist lcc,cdlp > results/csr3/analytics/dota-league-analytics-csr3
    ./build/gfe_driver -G datasets/com-friendster.ungraph.el -u -l csr3 --load -w $(nproc) -r $(nproc) -R 5 --blacklist lcc,cdlp > results/csr3/analytics/com-friendster.ungraph-analytics-csr3
    ./build/gfe_driver -G datasets/com-lj.ungraph.el -u -l csr3 --load -w $(nproc) -r $(nproc) -R 5 --blacklist lcc,cdlp > results/csr3/analytics/com-lj.ungraph-analytics-csr3
    ./build/gfe_driver -G datasets/com-orkut.ungraph.el -u -l csr3 --load -w $(nproc) -r $(nproc) -R 5 --blacklist lcc,cdlp > results/csr3/analytics/com-orkut.ungraph-analytics-csr3
fi