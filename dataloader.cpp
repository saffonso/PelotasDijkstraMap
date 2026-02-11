#include "dataloader.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <iostream>

Dataloader::Dataloader(const std::string& path) : datapath(path) {}

QJsonObject Dataloader::loadLabelToNodes(){
    QString filePath = QString::fromStdString(datapath+"/label_to_nodes.json");
    QFile file(filePath);

    if(!file.open(QIODevice::ReadOnly)){
        std::cerr << "Erro ao abrir o JSON" << filePath.toStdString() << std::endl;
        return QJsonObject();
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data,&parseError);

    if (parseError.error != QJsonParseError::NoError) {
        std::cerr << "Erro ao parsear label_to_nodes.json: " << parseError.errorString().toStdString() << std::endl;
        return QJsonObject();
    }

    if (!doc.isObject()) {
        std::cerr << "label_to_nodes.json não contém um objeto" << std::endl;
        return QJsonObject();
    }

    QJsonObject labelsJson = doc.object();
    std::cout << "Json carregado com " <<labelsJson.size() << " entradas." << std::endl;

    return labelsJson;
}

void Dataloader::makeTrie(Trie& trie,const QJsonObject& labelsJson){
    if(labelsJson.isEmpty()){
        std::cerr << "Json vazio." << std::endl;
        return;
    }

    size_t count = 0;
    for(auto obj = labelsJson.begin(); obj != labelsJson.end(); obj++){
        std::string label = obj.key().toStdString();
        std::vector<long long> nodeIds;

        QJsonArray idsArray = obj.value().toArray();
        for(const QJsonValue& id : std::as_const(idsArray)){
            nodeIds.push_back(static_cast<long long>(id.toDouble()));
        }

        trie.insert(label, nodeIds);
    }
}

QJsonArray Dataloader::loadNodesData() {
    QString filePath = QString::fromStdString(datapath + "/nodes.json");
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        std::cerr << "Erro ao abrir nodes.json" << std::endl;
        return QJsonArray();
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.array();
}

QJsonArray Dataloader::loadEdgesData() {
    QString filePath = QString::fromStdString(datapath + "/edges.json");
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        std::cerr << "Erro ao abrir edges.json" << std::endl;
        return QJsonArray();
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.array();
}

void Dataloader::fillGraph(Grafo& grafo, const QJsonArray& nodesArray, const QJsonArray& edgesArray) {
    for (const QJsonValue& value : nodesArray) {
        QJsonObject node = value.toObject();
        long long id = static_cast<long long>(node["id"].toDouble());
        double x = node["x"].toDouble(); 
        double y = node["y"].toDouble(); 
        
        grafo.addNode(id, x, y);
    }

    for (const QJsonValue& value : edgesArray) {
        QJsonObject edge = value.toObject();
        long long u = static_cast<long long>(edge["u"].toDouble());
        long long v = static_cast<long long>(edge["v"].toDouble());
        
        double length = edge["data"].toObject()["length"].toDouble();

        grafo.addEdge(u, v, length);
    }
}
