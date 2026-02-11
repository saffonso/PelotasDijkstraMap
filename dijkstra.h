#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "grafo.h"
#include <vector>
#include <limits>

struct PathResult {
    std::vector<long long> path;  
    double totalDistance;          
    bool found;                    
    
    PathResult() : totalDistance(0), found(false) {}
};

class Dijkstra {
public:
    static PathResult shortestPath(const Grafo& graph, long long origin, long long destination);
};

#endif
