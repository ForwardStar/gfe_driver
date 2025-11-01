python3 downloader.py
sed -i '/^#/d' datasets/*.txt
sed -i -E 's/[[:space:]]+/ /g' datasets/*.txt
mv datasets/twitter-2010.txt datasets/twitter-2010.el
mv datasets/com-friendster.ungraph.txt datasets/com-friendster.ungraph.el
mv datasets/com-lj.ungraph.txt datasets/com-lj.ungraph.el
mv datasets/com-orkut.ungraph.txt datasets/com-orkut.ungraph.el