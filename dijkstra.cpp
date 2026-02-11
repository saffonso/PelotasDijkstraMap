#include "dijkstra.h"
#include <queue>
#include <unordered_map>
#include <algorithm>

PathResult Dijkstra::shortestPath(const Grafo& graph, long long origin, long long destination) {
    PathResult result;
    
    if (!graph.hasNode(origin) || !graph.hasNode(destination)) {
        return result;
    }
    
    std::unordered_map<long long, double> dist;
    
    std::unordered_map<long long, long long> predecessor;
    
    // Min-heap
    using PQElement = std::pair<double, long long>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;
    
    
    dist[origin] = 0;
    pq.push({0, origin});
    
    while (!pq.empty()) {
        auto [currentDist, currentNode] = pq.top();
        pq.pop();
        
        if (currentNode == destination) {
            result.found = true;
            result.totalDistance = currentDist;
            
            // Reconstrói o caminhot
            long long node = destination;
            while (node != origin) {
                result.path.push_back(node);
                node = predecessor[node];
            }
            result.path.push_back(origin);
            std::reverse(result.path.begin(), result.path.end());
            
            return result;
        }
        
        
        if (dist.count(currentNode) && currentDist > dist[currentNode]) {
            continue;
        }
        
    
        for (const auto& [neighbor, weight] : graph.getNeighbors(currentNode)) {
            double newDist = currentDist + weight;
            
            if (!dist.count(neighbor) || newDist < dist[neighbor]) {
                dist[neighbor] = newDist;
                predecessor[neighbor] = currentNode;
                pq.push({newDist, neighbor});
            }
        }
    }
    
    return result;
}
