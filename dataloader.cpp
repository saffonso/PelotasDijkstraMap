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
