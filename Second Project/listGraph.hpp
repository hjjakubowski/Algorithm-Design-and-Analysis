#include <iostream>
#include <vector>
#include <unordered_map>

template<typename Vertex>
class List_Graph {
private:
	std::vector<Vertex> nodes;
	std::unordered_map<Vertex, int> idx;
	std::vector<std::vector<Vertex>> adj_list;
public:
	List_Graph(const std::vector<Vertex>& nodes);
	void addEdge(const Vertex& u, const Vertex& v);
	std::vector<Vertex> neighbours(const Vertex& u) const;
	bool hasEdge(const Vertex& u, const Vertex& v) const;
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
void List_Graph<Vertex>::addEdge(const Vertex& u, const Vertex& v) {
	int iu = idx.at(u);
	int iv = idx.at(v);
	adj_list[iu].push_back(iv);
	adj_list[iv].push_back(iu);
}

template<typename Vertex>
std::vector<Vertex> List_Graph<Vertex>::neighbours(const Vertex& u) const {
	int iu = idx.at(u);
	std::vector<Vertex> out;
	for (int iv : adj_list[iu]) {
		out.push_back(nodes[iv]);
	}
	return out;
}

template<typename Vertex>
bool List_Graph<Vertex>::hasEdge(const Vertex& u, const Vertex& v) const {
	int iu = idx.at(u);
	int iv = idx.at(v);
	return std::find(adj_list[iu].begin(), adj_list[iu].end(), iv) != adj_list[iu].end() ||
		std::find(adj_list[iv].begin(), adj_list[iv].end(), iu) != adj_list[iv].end(); //Do sprawdzenia
}