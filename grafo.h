#ifndef GRAFO_H
#define GRAFO_H

#include <unordered_map>
#include <vector>

class Node{
public: 
    long long id;
    double x;
    double y;

    Node() : id(0), x(0), y(0) {}
    Node(long long id, double x, double y) : id(id), x(x), y(y){}
};

class Grafo
{
private:
    std::unordered_map<long long, Node> nodes;

    std::unordered_map<long long, std::vector<std::pair<long long, double>>> infoList;
public:
    Grafo() = default;

    void addNode(long long id, double x, double y);

    void addEdge(long long u, long long v, double length);

    const std::vector<std::pair<long long, double>>& getNeighbors(long long nodeId) const;

    bool hasNode(long long nodeId) const;

    const Node& getNode(long long nodeId) const;

};

#endif
