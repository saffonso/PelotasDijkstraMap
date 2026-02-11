#ifndef DATALOADER_H
#define DATALOADER_H

#include "grafo.h"
#include "trie.h"
#include <qjsonobject.h>
#include <string>
#include <unordered_map>

class Dataloader
{
private:
    std::string datapath;
public:
    Dataloader(const std::string& path);

    bool loadNodes(Grafo& grafo);

    bool loadEdges(Grafo& grafo);

    QJsonObject loadLabelToNodes();

    bool loadNodeToLabel(std::unordered_map<long long, std::string>& nodesToLabel);

    void makeTrie(Trie& trie,const QJsonObject& labelsJson);

};

#endif // DATALOADER_H
