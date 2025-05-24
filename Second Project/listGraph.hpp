#include <iostream>
#include <vector>
#include <unordered_map>
#include <limits>
#include <algorithm>

template<typename Vertex>
class List_Graph {
private:
    std::vector<Vertex> nodes;
    std::unordered_map<Vertex, int> idx;
    std::vector<std::vector<std::pair<int, double>>> adj_list; // (indeks s¹siada, waga)
public:
    List_Graph(const std::vector<Vertex>& nodes);
    void addEdge(const Vertex& u, const Vertex& v, double weight);
    std::vector<std::pair<Vertex, double>> neighbours(const Vertex& u) const;
    bool hasEdge(const Vertex& u, const Vertex& v) const;
    double getWeight(const Vertex& u, const Vertex& v) const;
};

template<typename Vertex>
List_Graph<Vertex>::List_Graph(const std::vector<Vertex>& nodes) : nodes(nodes) {
    int n = nodes.size();
    adj_list.resize(n);
    for (int i = 0; i < n; ++i) {
        idx[nodes[i]] = i;
    }
}

template<typename Vertex>
void List_Graph<Vertex>::addEdge(const Vertex& u, const Vertex& v, double weight) {
    int iu = idx.at(u);
    int iv = idx.at(v);
    adj_list[iu].emplace_back(iv, weight);
    adj_list[iv].emplace_back(iu, weight); // dla grafu nieskierowanego
}

template<typename Vertex>
std::vector<std::pair<Vertex, double>> List_Graph<Vertex>::neighbours(const Vertex& u) const {
    int iu = idx.at(u);
    std::vector<std::pair<Vertex, double>> out;
    for (const auto& p : adj_list[iu]) {
        out.emplace_back(nodes[p.first], p.second);
    }
    return out;
}

template<typename Vertex>
bool List_Graph<Vertex>::hasEdge(const Vertex& u, const Vertex& v) const {
    int iu = idx.at(u);
    int iv = idx.at(v);
    for (const auto& p : adj_list[iu]) {
        if (p.first == iv) return true;
    }
    for (const auto& p : adj_list[iv]) {
        if (p.first == iu) return true;
    }
    return false;
}

template<typename Vertex>
double List_Graph<Vertex>::getWeight(const Vertex& u, const Vertex& v) const {
    int iu = idx.at(u);
    int iv = idx.at(v);
    for (const auto& p : adj_list[iu]) {
        if (p.first == iv) return p.second;
    }
    return std::numeric_limits<double>::infinity(); // brak krawêdzi
}
