#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <limits>
#include <algorithm>
#include <queue>
#include "matrixGraph.hpp"
#include "listGraph.hpp"

// Pomocnicza funkcja do generowania krawędzi z losowymi wagami
std::vector<std::tuple<int, int, double>> generate_random_edges(int V, double density, std::mt19937& rng) {
    std::vector<std::tuple<int, int, double>> edges;
    int max_edges = V * (V - 1) / 2;
    int edge_count = static_cast<int>(density * max_edges);

    std::vector<std::pair<int, int>> possible_edges;
    for (int i = 0; i < V; ++i) {
        for (int j = i + 1; j < V; ++j) {
            possible_edges.emplace_back(i, j);
        }
    }
    std::shuffle(possible_edges.begin(), possible_edges.end(), rng);

    std::uniform_real_distribution<double> weight_dist(1.0, static_cast<double>(std::numeric_limits<int>::max()));

    for (int i = 0; i < edge_count; ++i) {
        int u = possible_edges[i].first;
        int v = possible_edges[i].second;
        double w = weight_dist(rng);
        edges.emplace_back(u, v, w);
    }
    return edges;
}

// Dijkstra dla Matrix_Graph - all-pairs shortest path 
std::vector<double> dijkstra_matrix_all(const Matrix_Graph<int>& graph, int src, int V) {
    std::vector<double> dist(V, std::numeric_limits<double>::infinity());
    std::vector<bool> visited(V, false);
    dist[src] = 0.0;
    using P = std::pair<double, int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.emplace(0.0, src);

    while (!pq.empty()) {
        int u = pq.top().second;
        double d = pq.top().first;
        pq.pop();
        if (visited[u]) continue;
        visited[u] = true;
        for (const auto& nb : graph.neighbours(u)) {
            int v = nb.first;
            double w = nb.second;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.emplace(dist[v], v);
            }
        }
    }
    return dist;
}

// Dijkstra dla Matrix_Graph - single-pair shortest path
std::vector<int> dijkstra_matrix_path(const Matrix_Graph<int>& graph, int src, int dest, int V) {
    std::vector<double> dist(V, std::numeric_limits<double>::infinity());
    std::vector<int> prev(V, -1);
    std::vector<bool> visited(V, false);
    dist[src] = 0.0;
    using P = std::pair<double, int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.emplace(0.0, src);

    while (!pq.empty()) {
        int u = pq.top().second;
        double d = pq.top().first;
        pq.pop();
        if (visited[u]) continue;
        visited[u] = true;
        for (const auto& nb : graph.neighbours(u)) {
            int v = nb.first;
            double w = nb.second;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.emplace(dist[v], v);
            }
        }
    }
    std::vector<int> path;
    if (dist[dest] == std::numeric_limits<double>::infinity()) return path;
    for (int at = dest; at != -1; at = prev[at]) path.push_back(at);
    std::reverse(path.begin(), path.end());
    return path;
}

// Dijkstra dla List_Graph - all-pairs shortest path 
std::vector<double> dijkstra_list_all(const List_Graph<int>& graph, int src, int V) {
    std::vector<double> dist(V, std::numeric_limits<double>::infinity());
    std::vector<bool> visited(V, false);
    dist[src] = 0.0;
    using P = std::pair<double, int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.emplace(0.0, src);

    while (!pq.empty()) {
        int u = pq.top().second;
        double d = pq.top().first;
        pq.pop();
        if (visited[u]) continue;
        visited[u] = true;
        for (const auto& nb : graph.neighbours(u)) {
            int v = nb.first;
            double w = nb.second;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.emplace(dist[v], v);
            }
        }
    }
    return dist;
}

// Dijkstra dla List_Graph - single-pair shortest path
std::vector<int> dijkstra_list_path(const List_Graph<int>& graph, int src, int dest, int V) {
    std::vector<double> dist(V, std::numeric_limits<double>::infinity());
    std::vector<int> prev(V, -1);
    std::vector<bool> visited(V, false);
    dist[src] = 0.0;
    using P = std::pair<double, int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.emplace(0.0, src);

    while (!pq.empty()) {
        int u = pq.top().second;
        double d = pq.top().first;
        pq.pop();
        if (visited[u]) continue;
        visited[u] = true;
        for (const auto& nb : graph.neighbours(u)) {
            int v = nb.first;
            double w = nb.second;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.emplace(dist[v], v);
            }
        }
    }
    std::vector<int> path;
    if (dist[dest] == std::numeric_limits<double>::infinity()) return path;
    for (int at = dest; at != -1; at = prev[at]) path.push_back(at);
    std::reverse(path.begin(), path.end());
    return path;
}


void benchmark_dijkstra() {
    std::vector<int> sizes = { 10, 50, 100, 500, 1000 };
    std::vector<double> densities = { 0.25, 0.5, 0.75, 1.0 };
    std::random_device rd;
    std::mt19937 rng(rd());

    for (int V : sizes) {
        for (double density : densities) {
            std::vector<int> vertices(V);
            for (int i = 0; i < V; ++i) vertices[i] = i;

            double matrix_all_sum = 0, matrix_path_sum = 0;
            double list_all_sum = 0, list_path_sum = 0;
            int repetitions = 100;
            for (int rep = 0; rep < repetitions; ++rep) {
                auto edges = generate_random_edges(V, density, rng);

                Matrix_Graph<int> mgraph(vertices);
                List_Graph<int> lgraph(vertices);
                for (const auto& e : edges) {
                    int u, v;
                    double w;
                    std::tie(u, v, w) = e;
                    mgraph.addEdge(u, v, w);
                    lgraph.addEdge(u, v, w);
                }

                auto start = std::chrono::high_resolution_clock::now();
                auto dist_matrix = dijkstra_matrix_all(mgraph, 0, V);
                auto end = std::chrono::high_resolution_clock::now();
                matrix_all_sum += std::chrono::duration<double, std::milli>(end - start).count();

                start = std::chrono::high_resolution_clock::now();
                auto path_matrix = dijkstra_matrix_path(mgraph, 0, V - 1, V);
                end = std::chrono::high_resolution_clock::now();
                matrix_path_sum += std::chrono::duration<double, std::milli>(end - start).count();

                start = std::chrono::high_resolution_clock::now();
                auto dist_list = dijkstra_list_all(lgraph, 0, V);
                end = std::chrono::high_resolution_clock::now();
                list_all_sum += std::chrono::duration<double, std::milli>(end - start).count();

                start = std::chrono::high_resolution_clock::now();
                auto path_list = dijkstra_list_path(lgraph, 0, V - 1, V);
                end = std::chrono::high_resolution_clock::now();
                list_path_sum += std::chrono::duration<double, std::milli>(end - start).count();
            }

            std::cout << "Wierzcholki: " << V << ", Gestosc: " << (density * 100) << "%\n";
            std::cout << "MatrixGraph - Dijkstra all: " << (matrix_all_sum / repetitions) << " ms\n";
            std::cout << "MatrixGraph - Dijkstra path: " << (matrix_path_sum / repetitions) << " ms\n";
            std::cout << "ListGraph   - Dijkstra all: " << (list_all_sum / repetitions) << " ms\n";
            std::cout << "ListGraph   - Dijkstra path: " << (list_path_sum / repetitions) << " ms\n";
            std::cout << "---------------------------------------------\n";
        }
    }
}

void simple_driver_demo() {
    int V = 10;
    double density = 0.25;
    int src = 0, dest = 9;
    std::vector<int> vertices(V);
    for (int i = 0; i < V; ++i) vertices[i] = i;
    std::random_device rd;
    std::mt19937 rng(rd());

    std::vector<std::tuple<int, int, double>> edges;
    int max_edges = V * (V - 1) / 2;
    int edge_count = static_cast<int>(density * max_edges);
    std::vector<std::pair<int, int>> possible_edges;
    for (int i = 0; i < V; ++i) {
        for (int j = i + 1; j < V; ++j) {
            possible_edges.emplace_back(i, j);
        }
    }
    std::shuffle(possible_edges.begin(), possible_edges.end(), rng);
    std::uniform_int_distribution<int> weight_dist(1, 10);
    for (int i = 0; i < edge_count; ++i) {
        int u = possible_edges[i].first;
        int v = possible_edges[i].second;
        double w = static_cast<double>(weight_dist(rng));
        edges.emplace_back(u, v, w);
    }

    Matrix_Graph<int> mgraph(vertices);
    List_Graph<int> lgraph(vertices);
    for (const auto& e : edges) {
        int u, v;
        double w;
        std::tie(u, v, w) = e;
        mgraph.addEdge(u, v, w);
        lgraph.addEdge(u, v, w);
    }

    // ======= MATRIX_GRAPH: Sąsiedzi i wagi =======
    std::cout << "=== MATRIX_GRAPH: Sasiedzi i wagi ===\n";
    for (int i = 0; i < V; ++i) {
        std::cout << "Wierzcholek " << i << " sasiaduje z: ";
        auto nbs = mgraph.neighbours(i);
        for (const auto& nb : nbs) {
            std::cout << nb.first << "(waga: " << nb.second << ") ";
        }
        std::cout << "\n";
    }std::cout << "\n";

    // ======= MATRIX_GRAPH: Dijkstra (single-pair shortest path) =======
    std::cout << "=== MATRIX_GRAPH: Dijkstra (single-pair shortest path) ===\n";
    auto path = dijkstra_matrix_path(mgraph, src, dest, V);
    if (path.empty()) {
        std::cout << "Brak sciezki z " << src << " do " << dest << ".\n";
    }
    else {
        std::cout << "Najkrotsza sciezka z " << src << " do " << dest << ": ";
        for (size_t i = 0; i < path.size(); ++i) {
            std::cout << path[i];
            if (i + 1 < path.size()) std::cout << " -> ";
        }
        double total_weight = 0;
        for (size_t i = 1; i < path.size(); ++i)
            total_weight += mgraph.getWeight(path[i - 1], path[i]);
        std::cout << "\nDlugosc sciezki: " << total_weight << "\n";
    }
    std::cout << "-----------------------------------------\n\n";

    // ======= MATRIX_GRAPH: Dijkstra (all-pairs shortest path) =======
    std::cout << "=== MATRIX_GRAPH: Dijkstra (all-pairs shortest path from " << src << ") ===\n";
    auto dist_matrix = dijkstra_matrix_all(mgraph, src, V);
    for (int i = 0; i < V; ++i) {
        std::cout << "Odleglosc z " << src << " do " << i << ": ";
        if (dist_matrix[i] == std::numeric_limits<double>::infinity()) {
            std::cout << "brak sciezki";
        }
        else {
            std::cout << dist_matrix[i];
        }
        std::cout << "\n";
    }
    std::cout << "-----------------------------------------\n\n";

    // ======= LIST_GRAPH: Sąsiedzi i wagi =======
    std::cout << "=== LIST_GRAPH: Sasiedzi i wagi ===\n";
    for (int i = 0; i < V; ++i) {
        std::cout << "Wierzcholek " << i << " sasiaduje z: ";
        auto nbs = lgraph.neighbours(i);
        for (const auto& nb : nbs) {
            std::cout << nb.first << "(waga: " << nb.second << ")  ";
        }
        std::cout << "\n";
    }std::cout << "\n";

    // ======= LIST_GRAPH: Dijkstra (single-pair shortest path) =======
    std::cout << "=== LIST_GRAPH: Dijkstra (single-pair shortest path) ===\n";
    auto lpath = dijkstra_list_path(lgraph, src, dest, V);
    if (lpath.empty()) {
        std::cout << "Brak sciezki z " << src << " do " << dest << ".\n";
    }
    else {
        std::cout << "Najkrotsza sciezka z " << src << " do " << dest << ": ";
        for (size_t i = 0; i < lpath.size(); ++i) {
            std::cout << lpath[i];
            if (i + 1 < lpath.size()) std::cout << " -> ";
        }
        double total_weight = 0;
        for (size_t i = 1; i < lpath.size(); ++i)
            total_weight += lgraph.getWeight(lpath[i - 1], lpath[i]);
        std::cout << "\nDlugosc sciezki: " << total_weight << "\n";
    }
    std::cout << "-----------------------------------------\n\n";

    // ======= LIST_GRAPH: Dijkstra (all-pairs shortest path) =======
    std::cout << "=== LIST_GRAPH: Dijkstra (all-pairs shortest path from " << src << ") ===\n";
    auto dist_list = dijkstra_list_all(lgraph, src, V);
    for (int i = 0; i < V; ++i) {
        std::cout << "Odleglosc z " << src << " do " << i << ": ";
        if (dist_list[i] == std::numeric_limits<double>::infinity()) {
            std::cout << "brak sciezki";
        }
        else {
            std::cout << dist_list[i];
        }
        std::cout << "\n";
    }
    std::cout << "-----------------------------------------\n\n";
}


int main() {
    benchmark_dijkstra();
    simple_driver_demo();
    return 0;
}
