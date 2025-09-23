#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <unordered_set>

// Function 1: Read vertices file and connect adjacent vertices
std::vector<std::pair<int, int>> readVerticesFile(const std::string& filename) {
    std::ifstream infile(filename);
    std::vector<int> vertices;
    std::vector<std::pair<int, int>> edges;

    int vertex;
    while (infile >> vertex) {
        vertices.push_back(vertex);
    }

    // Connect adjacent vertices
    for (size_t i = 0; i < vertices.size(); ++i) {
        edges.emplace_back(vertices[i], vertices[(i + 1) % vertices.size()]);
    }

    return edges;
}

// Function 2: Read edges file and collect edges
std::vector<std::pair<int, int>> readEdgesFile(const std::string& filename) {
    std::ifstream infile(filename);
    std::unordered_set<int> vertex_set;

    int u, v;
    while (infile >> u >> v) {
        vertex_set.insert(u);
        vertex_set.insert(v);
    }

    std::vector<int> vertices;
    for (const auto& vertex : vertex_set) {
        vertices.push_back(vertex);
    }

    std::vector<std::pair<int, int>> edges;
    for (size_t i = 0; i < vertices.size(); ++i) {
        edges.emplace_back(vertices[i], vertices[(i + 1) % vertices.size()]);
    }

    return edges;
}

int main() {
    std::vector<std::string> file_paths = {
        "datasets/graph500-24.v",
        "datasets/uniform-24.v",
        "datasets/dota-league.v",
        "datasets/com-lj.ungraph.el",
        "datasets/com-orkut.ungraph.el",
        "datasets/twitter-2010.el"
    };

    // Process in parallel
    #pragma omp parallel for
    for (const auto& file_path : file_paths) {
        std::vector<std::pair<int, int>> edges;
        if (file_path.find(".v") != std::string::npos) {
            edges = readVerticesFile(file_path);
        } else if (file_path.find(".el") != std::string::npos) {
            edges = readEdgesFile(file_path);
        }
        // Substitute .v to .el, add .vertices before .el
        std::string output_file = file_path;
        if (output_file.find(".v") != std::string::npos) {
            output_file.replace(output_file.find(".v"), 2, ".el");
            output_file.insert(output_file.find(".el"), ".vertices");
        } else if (output_file.find(".el") != std::string::npos) {
            output_file.insert(output_file.find(".el"), ".vertices");
        }
        std::ofstream outfile(output_file);
        // Write edges to output file
        for (const auto& edge : edges) {
            outfile << edge.first << " " << edge.second << "\n";
        }
    }

    return 0;
}