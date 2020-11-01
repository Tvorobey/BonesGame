#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//Qt
#include <QMainWindow>
#include <QStandardItemModel>
#include <QItemSelectionModel>
//Local
#include "HellowPopUp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setModel(QSharedPointer<QStandardItemModel> model);
signals:
    void startGame(int cellCount);
    void stoneSelected(const QItemSelection &selected, const QItemSelection &deselected);
public slots:
    void clearSelections();
private:
    void createConnection();
private:
    Ui::MainWindow *ui;
    HellowPopUp* popup;
    QSharedPointer<QStandardItemModel>  m_stoneModel;
    QItemSelectionModel*    m_selectionModel;
};
#endif // MAINWINDOW_H
