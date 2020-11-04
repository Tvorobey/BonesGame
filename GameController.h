#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

//Qt
#include <QObject>
#include <QSharedPointer>
#include <QStandardItemModel>
#include <QItemSelection>

using From_To = QPair<int, int>;
using ColumnToDelete = QVector<QPair<int, From_To>>;
using RowToDelete = QVector<QPair<int, From_To>>;

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
    void swapCells(const QModelIndex& first, const QModelIndex& second);
    void scanScene(const QModelIndex &from = QModelIndex(),
                    const QModelIndex &to = QModelIndex(),
                    bool clicked = true);
    void deleteMatches(const ColumnToDelete& columnToDelete, const RowToDelete& rowToDelete);
private:
    QSharedPointer<QStandardItemModel>  m_model;
};

#endif // GAMECONTROLLER_H
