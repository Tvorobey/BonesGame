#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

//Qt
#include <QObject>
#include <QSharedPointer>
#include <QStandardItemModel>
#include <QItemSelection>

class StoneModel;

class GameController : public QObject
{
    Q_OBJECT
public:
    explicit GameController(QObject *parent = nullptr);
    void setModel(QSharedPointer<QStandardItemModel> model);
signals:
    void clearSelection();
public slots:
    void onStartGame(int cellCount);
    void onStoneSelected(const QItemSelection &selected, const QItemSelection &deselected);
private:
    QSharedPointer<QStandardItemModel>  m_model;
    QModelIndexList                     m_selectedStone;
    QModelIndexList                     m_deselectedStone;
};

#endif // GAMECONTROLLER_H
