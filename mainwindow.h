#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTimer>
#include "trie.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // Getters para obter dados selecionados
    QString getSelectedFromName() const { return selectedFromName; }
    QString getSelectedToName() const { return selectedToName; }
    std::vector<long long> getSelectedFromNodeIds() const { return selectedFromNodeIds; }
    std::vector<long long> getSelectedToNodeIds() const { return selectedToNodeIds; }

private slots:
    void onFromTextChanged(const QString& text);
    void onToTextChanged(const QString& text);
    void performFromAutocomplete();
    void performToAutocomplete();
    void onFromItemClicked(QListWidgetItem* item);
    void onToItemClicked(QListWidgetItem* item);
    void onVerRotaClicked();

private:
    void showPopup(QListWidget* popup, const std::vector<std::pair<std::string, std::vector<long long>>>& results, QLineEdit* lineEdit);
    void hidePopup(QListWidget* popup);
    
    Ui::MainWindow *ui;
    
    Trie* trie;
    
    QListWidget* fromPopup;
    QListWidget* toPopup;
    
    QTimer* fromDebounceTimer;
    QTimer* toDebounceTimer;
    
    QString fromCurrentText;
    QString toCurrentText;
    std::vector<std::pair<std::string, std::vector<long long>>> fromResults;
    std::vector<std::pair<std::string, std::vector<long long>>> toResults;
    
    QString selectedFromName;
    QString selectedToName;
    std::vector<long long> selectedFromNodeIds;
    std::vector<long long> selectedToNodeIds;
};
#endif // MAINWINDOW_H
