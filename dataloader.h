#ifndef DATALOADER_H
#define DATALOADER_H

#include "grafo.h"
#include "trie.h"
#include <qjsonobject.h>
#include <string>
#include <unordered_map>
#include <QJsonArray>

class Dataloader
{
private:
    std::string datapath;
public:
    Dataloader(const std::string& path);

    QJsonArray loadNodesData();
    QJsonArray loadEdgesData();

    void fillGraph(Grafo& grafo, const QJsonArray& nodes, const QJsonArray& edges);

    QJsonObject loadLabelToNodes();

    bool loadNodeToLabel(std::unordered_map<long long, std::string>& nodesToLabel);

    void makeTrie(Trie& trie,const QJsonObject& labelsJson);

};

#endif // DATALOADER_H
