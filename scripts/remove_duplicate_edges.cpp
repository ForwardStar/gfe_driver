#include <bits/stdc++.h>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <edge file>" << std::endl;
        return 1;
    }
    ifstream fin(argv[1]);
    if (!fin.is_open()) {
        cerr << "Error: cannot open edge file " << argv[1] << " for reading" << std::endl;
        return 1;
    }
    set<pair<uint64_t, uint64_t>> edge_set;
    uint64_t u, v;
    while (fin >> u >> v) {
        if (u > v) {
            edge_set.insert({v, u});
        } else {
            edge_set.insert({u, v});
        }
    }
    fin.close();
    ofstream fout(argv[1]);
    if (!fout.is_open()) {
        cerr << "Error: cannot open edge file " << argv[1] << " for writing" << std::endl;
        return 1;
    }
    for (const auto& edge : edge_set) {
        fout << edge.first << " " << edge.second << std::endl;
    }
    fout.close();
    return 0;
}