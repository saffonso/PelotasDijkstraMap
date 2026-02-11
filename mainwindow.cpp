#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "dataloader.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDebug>

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
    Dataloader loader("C:\\Users\\santi\\Downloads");
    QJsonObject labelsJson = loader.loadLabelToNodes();
    
    if (labelsJson.isEmpty()) {
        QMessageBox::warning(this, "Aviso",
            "Não foi possível carregar o arquivo label_to_nodes.json.\n"
            "Autocomplete não estará disponível.\n"
            "Coloque o arquivo em: ./data/label_to_nodes.json");
    } else {
        loader.makeTrie(*trie, labelsJson);
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
    qDebug() << "=== VEJA ROTA ===";
    
    qDebug() << "De:" << selectedFromName << "(" << selectedFromNodeIds.size() << "nós)";
    for (long long nodeId : selectedFromNodeIds) {
        qDebug() << "  " << nodeId;
    }
    
    qDebug() << "Para:" << selectedToName << "(" << selectedToNodeIds.size() << "nós)";
    for (long long nodeId : selectedToNodeIds) {
        qDebug() << "  " << nodeId;
    }
    
    qDebug() << "=================";
}
