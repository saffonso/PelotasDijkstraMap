#include "grafo.h"

void Grafo::addNode(long long id, double x, double y) {
    nodes[id] = Node(id, x, y);
    
    if (infoList.find(id) == infoList.end()) {
        infoList[id] = std::vector<std::pair<long long, double>>();
    }
}

void Grafo::addEdge(long long u, long long v, double length) {
    infoList[u].push_back({v, length});
    
}

const std::vector<std::pair<long long, double>>& Grafo::getNeighbors(long long nodeId) const {
    static const std::vector<std::pair<long long, double>> listaVazia;
    
    auto it = infoList.find(nodeId);
    if (it != infoList.end()) {
        return it->second;
    }
    
    return listaVazia;
}

bool Grafo::hasNode(long long nodeId) const {
    return nodes.find(nodeId) != nodes.end();
}

const Node& Grafo::getNode(long long nodeId) const {
    return nodes.at(nodeId);
}

