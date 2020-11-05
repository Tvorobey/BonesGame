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
    /*!
     * \brief onStoneSelected - при выборе шарика
     * \param selected - куда
     * \param deselected - откуда
     */
    void onStoneSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void swapCells(const QModelIndex& first, const QModelIndex& second);
    /*!
     * \brief scanScene - сканирует сцену на предмет стоящих подряд три или более
     * одинаковых шариков
     * \param from - ячейка, которую пользователь выбрал для свапа
     * \param to - ячейка, в которую пользователь выбрал свап
     * \param clicked - true - кликнул пользователь, false - мы сами заполнили поле
     */
    void scanScene( bool clicked = true,
                    const QModelIndex &from = QModelIndex(),
                    const QModelIndex &to = QModelIndex());
    /*!
     * \brief deleteMatches - удаление одинаковых шариков
     * \param columnToDelete - номер строки, (колнка от которой начать удалять, колонка до которой)
     * \param rowToDelete - номер колонки, (строка от которой надо удалять, строка до которой надо удалять)
     */
    void deleteMatches(const ColumnToDelete& columnToDelete, const RowToDelete& rowToDelete);
    /*!
     * \brief shuffleDown - дроп шариков на свободные ячейки вниз
     */
    void shuffleDown();
    /*!
     * \brief floodFill - заполнение пустых ячеек поля
     */
    void floodFill();
    bool victoryCheck();
private:
    QSharedPointer<QStandardItemModel>  m_model;
};

#endif // GAMECONTROLLER_H
