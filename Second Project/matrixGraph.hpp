#include <iostream>
#include <vector>
#include <unordered_map>
#include <limits>

template<typename Vertex>
class Matrix_Graph {
    std::vector<Vertex> nodes;
    std::unordered_map<Vertex, int> idx;
    std::vector<std::vector<double>> adj_matrix; // macierz wag
public:
    Matrix_Graph(const std::vector<Vertex>& nodes);
    void addEdge(const Vertex& u, const Vertex& v, double weight);
    std::vector<std::pair<Vertex, double>> neighbours(const Vertex& u) const;
    bool hasEdge(const Vertex& u, const Vertex& v) const;
    double getWeight(const Vertex& u, const Vertex& v) const;
};

template<typename Vertex>
Matrix_Graph<Vertex>::Matrix_Graph(const std::vector<Vertex>& nodes) : nodes(nodes) {
    int n = nodes.size();
    adj_matrix.assign(n, std::vector<double>(n, std::numeric_limits<double>::infinity()));
    for (int i = 0; i < n; ++i) {
        idx[nodes[i]] = i;
    }
}

template<typename Vertex>
void Matrix_Graph<Vertex>::addEdge(const Vertex& u, const Vertex& v, double weight) {
    int iu = idx.at(u);
    int iv = idx.at(v);
    adj_matrix[iu][iv] = adj_matrix[iv][iu] = weight;
}

template<typename Vertex>
std::vector<std::pair<Vertex, double>> Matrix_Graph<Vertex>::neighbours(const Vertex& u) const {
    int iu = idx.at(u);
    std::vector<std::pair<Vertex, double>> out;
    for (int iv = 0; iv < (int)nodes.size(); ++iv) {
        if (adj_matrix[iu][iv] != std::numeric_limits<double>::infinity()) {
            out.emplace_back(nodes[iv], adj_matrix[iu][iv]);
        }
    }
    return out;
}

template<typename Vertex>
bool Matrix_Graph<Vertex>::hasEdge(const Vertex& u, const Vertex& v) const {
    return adj_matrix[idx.at(u)][idx.at(v)] != std::numeric_limits<double>::infinity();
}

template<typename Vertex>
double Matrix_Graph<Vertex>::getWeight(const Vertex& u, const Vertex& v) const {
    return adj_matrix[idx.at(u)][idx.at(v)];
}
