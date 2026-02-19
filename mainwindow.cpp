#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "dataloader.h"
#include "dijkstra.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDebug>
#include <QQuickItem>
#include <QVariant>
#include <QGeoCoordinate>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , trie(nullptr)
    , fromPopup(nullptr)
    , toPopup(nullptr)
{
    ui->setupUi(this);
    ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/QmlMaps.qml")));
    ui->quickWidget->show();

    trie = new Trie();
    QDir dir (QCoreApplication::applicationDirPath());

    dir.cdUp();
    dir.cdUp();

    QString dataPath = dir.filePath("data");
    qDebug() << dataPath;

    Dataloader loader(dataPath.toStdString());
    QJsonObject labelsJson = loader.loadLabelToNodes(); 
    
    QJsonArray nodesData = loader.loadNodesData();
    QJsonArray edgesData = loader.loadEdgesData();
    
    if (labelsJson.isEmpty()) {
        QMessageBox::warning(this, "Aviso",
            "Não foi possível carregar os dados.\n"
            "Verifique se a pasta 'data' está correta.");
    } else {
        loader.makeTrie(*trie, labelsJson);
        loader.fillGraph(grafo, nodesData, edgesData);
    }
    
    fromPopup = new QListWidget();
    fromPopup->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    fromPopup->setAttribute(Qt::WA_ShowWithoutActivating);
    fromPopup->setFocusPolicy(Qt::NoFocus);
    fromPopup->setMaximumHeight(200);
    fromPopup->setStyleSheet(
        "QListWidget {"
        "   border: 1px solid #ccc;"
        "   background-color: white;"
        "   selection-background-color: #0078d4;"
        "}"
        "QListWidget::item {"
        "   padding: 8px;"
        "   border-bottom: 1px solid #eee;"
        "   color: black;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #e5f3ff;"
        "}");
    fromPopup->hide();
    
    toPopup = new QListWidget();
    toPopup->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    toPopup->setAttribute(Qt::WA_ShowWithoutActivating);
    toPopup->setFocusPolicy(Qt::NoFocus);
    toPopup->setMaximumHeight(200);
    toPopup->setStyleSheet(fromPopup->styleSheet());
    toPopup->hide();
    
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &MainWindow::onFromTextChanged);
    connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &MainWindow::onToTextChanged);
    
    connect(fromPopup, &QListWidget::itemClicked, this, &MainWindow::onFromItemClicked);
    connect(toPopup, &QListWidget::itemClicked, this, &MainWindow::onToItemClicked);
    
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onVerRotaClicked);
}

MainWindow::~MainWindow()
{
    delete trie;
    delete fromPopup;
    delete toPopup;
    delete ui;
}

void MainWindow::onFromTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        hidePopup(fromPopup);
        fromResults.clear();
        fromResults.shrink_to_fit();
        return;
    }

    fromResults.clear();
    fromResults.shrink_to_fit();
    fromResults = trie->autocomplete(text.toStdString(), 5);
    
    showPopup(fromPopup, fromResults, ui->lineEdit);
}

void MainWindow::onToTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        hidePopup(toPopup);
        toResults.clear();
        toResults.shrink_to_fit();
        return;
    }
    
    toResults.clear();
    toResults.shrink_to_fit();
    toResults = trie->autocomplete(text.toStdString(), 5);
    
    showPopup(toPopup, toResults, ui->lineEdit_2);
}

void MainWindow::showPopup(QListWidget* popup, const std::vector<std::pair<std::string, std::vector<long long>>>& results, QLineEdit* lineEdit)
{
    popup->clear();
    
    if (results.empty()) {
        hidePopup(popup);
        return;
    }
    
    for (const auto& [name, nodeIds] : results) {
        popup->addItem(QString::fromStdString(name));
    }
    
    QPoint globalPos = lineEdit->mapToGlobal(QPoint(0, lineEdit->height()));
    popup->setGeometry(globalPos.x(), globalPos.y(), lineEdit->width(), 200);
    
    popup->show();
    popup->raise();
    
    lineEdit->setFocus();
}

void MainWindow::hidePopup(QListWidget* popup)
{
    popup->hide();
}

void MainWindow::onFromItemClicked(QListWidgetItem* item)
{
    int index = fromPopup->row(item);
    
    if (index >= 0 && index < static_cast<int>(fromResults.size())) {
        const auto& [name, nodeIds] = fromResults[index];
        
        selectedFromName = QString::fromStdString(name);
        selectedFromNodeIds = nodeIds;
        
        ui->lineEdit->blockSignals(true);
        ui->lineEdit->setText(QString::fromStdString(name));
        ui->lineEdit->blockSignals(false);
        
        hidePopup(fromPopup);  
    }
}

void MainWindow::onToItemClicked(QListWidgetItem* item)
{
    int index = toPopup->row(item);
    
    if (index >= 0 && index < static_cast<int>(toResults.size())) {
        const auto& [name, nodeIds] = toResults[index];
        
        selectedToName = QString::fromStdString(name);
        selectedToNodeIds = nodeIds;
        
        ui->lineEdit_2->blockSignals(true);
        ui->lineEdit_2->setText(QString::fromStdString(name));
        ui->lineEdit_2->blockSignals(false);
        
        hidePopup(toPopup);
    }
}

void MainWindow::onVerRotaClicked()
{
    
    if (selectedFromNodeIds.empty() || selectedToNodeIds.empty()) {
        QMessageBox::warning(this, "Aviso", "Selecione a origem e o destino primeiro.");
        return;
    }

    long long startNode = selectedFromNodeIds[0];
    long long endNode = selectedToNodeIds[0];
    
    PathResult result = Dijkstra::shortestPath(grafo, startNode, endNode);
    
    if (result.found) {
        
        drawPathOnMap(result.path);
        
        if (grafo.hasNode(startNode) && grafo.hasNode(endNode)) {
            const Node& s = grafo.getNode(startNode);
            const Node& e = grafo.getNode(endNode);
            
            QVariant startCoord = QVariant::fromValue(QGeoCoordinate(s.y, s.x));
            QVariant endCoord = QVariant::fromValue(QGeoCoordinate(e.y, e.x));
            
            QObject* rootObject = ui->quickWidget->rootObject();
            if (rootObject) {
                 QMetaObject::invokeMethod(rootObject, "setMarkers",
                                           Q_ARG(QVariant, startCoord),
                                           Q_ARG(QVariant, endCoord));
            }
        }

    } else {
        QMessageBox::warning(this, "Aviso", "Não foi possível encontrar um caminho entre estes pontos.");
    }
}

void MainWindow::drawPathOnMap(const std::vector<long long>& pathIds) {
    QVariantList pathCoordinates;
    
    for (long long id : pathIds) {
        if (grafo.hasNode(id)) {
            const Node& node = grafo.getNode(id);
            pathCoordinates.append(QVariant::fromValue(QGeoCoordinate(node.y, node.x)));
        }
    }
    
    QObject* rootObject = ui->quickWidget->rootObject();
    if (rootObject) {
         QMetaObject::invokeMethod(rootObject, "setPath",
                                   Q_ARG(QVariant, QVariant::fromValue(pathCoordinates)));
    } else {
        qDebug() << "Erro: RootObject do QML não encontrado!";
    }
}

