#include <bits/stdc++.h>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input.el>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    if (inputFile.size() < 3 || inputFile.substr(inputFile.size() - 3) != ".el") {
        cerr << "Error: input file must end with .el" << endl;
        return 1;
    }

    // Derive output filenames
    string baseName = inputFile.substr(0, inputFile.size() - 3);
    string vertexFile = baseName + ".v";
    string edgeFile = baseName + ".e";
    string propertiesFile = baseName + ".properties";

    // Read edges
    ifstream fin(inputFile);
    if (!fin.is_open()) {
        cerr << "Error: cannot open input file " << inputFile << endl;
        return 1;
    }

    unordered_map<long long, int> degree;
    vector<pair<long long, long long>> edges;
    unordered_set<long long> vertices;

    long long u, v;
    while (fin >> u >> v) {
        edges.emplace_back(u, v);
        degree[u]++;
        degree[v]++;
        vertices.insert(u);
        vertices.insert(v);
    }
    fin.close();

    // Find vertex with highest degree
    long long maxVertex = -1;
    int maxDegree = -1;
    for (auto& [vertex, deg] : degree) {
        if (deg > maxDegree) {
            maxDegree = deg;
            maxVertex = vertex;
        }
    }

    // Write vertex file
    ofstream foutV(vertexFile);
    if (!foutV.is_open()) {
        cerr << "Error: cannot open vertex file for writing" << endl;
        return 1;
    }
    for (auto vtx : vertices)
        foutV << vtx << "\n";
    foutV.close();

    // Write edge file with random weights
    ofstream foutE(edgeFile);
    if (!foutE.is_open()) {
        cerr << "Error: cannot open edge file for writing" << endl;
        return 1;
    }

    mt19937 gen(42);
    uniform_real_distribution<double> dist(0.000001, 1.0);

    for (auto& [a, b] : edges) {
        double w = dist(gen);
        foutE << a << " " << b << " " << w << "\n";
    }
    foutE.close();

    // Write properties file
    ofstream foutP(propertiesFile);
    if (!foutP.is_open()) {
        cerr << "Error: cannot open properties file for writing" << endl;
        return 1;
    }

    foutP << "# Properties file describing the " << baseName << " dataset\n"
          << "# Filenames of graph on local filesystem\n"
          << "graph." << baseName << ".vertex-file = " << baseName << ".v\n"
          << "graph." << baseName << ".edge-file = " << baseName << ".e\n\n"
          << "# Graph metadata for reporting purposes\n"
          << "graph." << baseName << ".meta.vertices = " << vertices.size() << "\n"
          << "graph." << baseName << ".meta.edges = " << edges.size() << "\n\n"
          << "# Properties describing the graph format\n"
          << "graph." << baseName << ".directed = false\n\n"
          << "# Description of graph properties\n"
          << "graph." << baseName << ".edge-properties.names = weight\n"
          << "graph." << baseName << ".edge-properties.types = real\n\n"
          << "# List of supported algorithms on the graph\n"
          << "graph." << baseName << ".algorithms = bfs, cdlp, lcc, pr, sssp, wcc\n\n\n"
          << "#\n# Per-algorithm properties describing the input parameters to each algorithm\n#\n\n"
          << "# Parameters for BFS\n"
          << "graph." << baseName << ".bfs.source-vertex = " << maxVertex << "\n\n"
          << "# Parameters for CDLP\n"
          << "graph." << baseName << ".cdlp.max-iterations = 10\n\n"
          << "# No parameters for LCC\n\n"
          << "# Parameters for PR\n"
          << "graph." << baseName << ".pr.damping-factor = 0.85\n"
          << "graph." << baseName << ".pr.num-iterations = 10\n\n"
          << "# Parameters for SSSP\n"
          << "graph." << baseName << ".sssp.weight-property = weight\n"
          << "graph." << baseName << ".sssp.source-vertex = " << maxVertex << "\n\n"
          << "# No parameters for WCC\n";

    foutP.close();

    cout << "Generated files:\n"
         << " - " << vertexFile << " (" << vertices.size() << " vertices)\n"
         << " - " << edgeFile << " (" << edges.size() << " edges)\n"
         << " - " << propertiesFile << " (source vertex = " << maxVertex << ")\n";

    return 0;
}
