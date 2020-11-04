#include "StoneModel.h"
#include "GameController.h"
#include "Global.h"

#include <QDebug>
#include <QRandomGenerator>
#include <QTimer>

using namespace GlobalDefines;

namespace
{
    const int REVERSE_SWAP_DELAY = 500;

    const QMap<StoneColor, QString> COLOR_TO_STR
    {
        {StoneColor::Red, "Red"},
        {StoneColor::Green, "Green"},
        {StoneColor::Blue, "blue"},
        {StoneColor::Yellow, "Yellow"},
        {StoneColor::Magenta, "Magenta"}
    };
}

GameController::GameController(QObject *parent) : QObject(parent)
{

}

void GameController::setModel(QSharedPointer<QStandardItemModel> model)
{
    m_model = model;
}

void GameController::onStartGame(int cellCount)
{
    m_model->setRowCount(cellCount);
    m_model->setColumnCount(cellCount);

    for (int i = 0; i < m_model->rowCount(); ++i)
    {
        for (int j = 0; j < m_model->columnCount(); ++j)
        {
            if ((i < 2) && (j < 2))
            {
                quint64 type = QRandomGenerator::securelySeeded().bounded(StoneColor::ColorCount);
                m_model->setData(m_model->index(i, j), type, Qt::DisplayRole);
            }

            if (j > 1)
            {
                int prevPrevType    = m_model->data(m_model->index(i, j - 2)).toInt();
                int prevType        = m_model->data(m_model->index(i, j - 2)).toInt();

                if (prevPrevType == prevType)
                {
                    while (true)
                    {
                       quint64 type = QRandomGenerator::securelySeeded().bounded(StoneColor::ColorCount);
                       if (type != prevType)
                       {
                            m_model->setData(m_model->index(i, j), type, Qt::DisplayRole);
                            break;
                       }
                    }
                }
                else
                {
                    quint64 type = QRandomGenerator::securelySeeded().bounded(StoneColor::ColorCount);
                    m_model->setData(m_model->index(i, j), type, Qt::DisplayRole);
                }
            }
            if (i > 1)
            {
                int prevPrevType    = m_model->data(m_model->index(i - 2, j)).toInt();
                int prevType        = m_model->data(m_model->index(i - 1, j)).toInt();

                if (prevPrevType == prevType)
                {
                    while (true)
                    {
                       quint64 type = QRandomGenerator::securelySeeded().bounded(StoneColor::ColorCount);

                       if (type != prevType)
                       {
                           if (m_model->data(m_model->index(i, j -1)).isValid())
                           {
                               int prevRowType = m_model->data(m_model->index(i, j -1)).toInt();

                               if (type != prevRowType)
                               {
                                   m_model->setData(m_model->index(i, j), type, Qt::DisplayRole);
                                   break;
                               }
                           }
                           else
                           {
                               m_model->setData(m_model->index(i, j), type, Qt::DisplayRole);
                               break;
                           }
                       }
                    }
                }
                else
                {
                    quint64 type = QRandomGenerator::securelySeeded().bounded(StoneColor::ColorCount);
                    m_model->setData(m_model->index(i, j), type, Qt::DisplayRole);
                }
            }
        }
    }
}

void GameController::onStoneSelected(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndex fromIndex;
    if (!deselected.indexes().isEmpty()) fromIndex = deselected.indexes().first();

    QModelIndex toIndex;
    if (!selected.indexes().isEmpty()) toIndex   = selected.indexes().first();

    // Проверяем был ли убран фокус с другой ячейки
    if (fromIndex.isValid() && toIndex.isValid())
    {
        // Проверим, что человеком была выбрана соседняя ячейка по вертикали или горизонтали
        int rowDelta    = fromIndex.row() - toIndex.row();
        int columnDelta = fromIndex.column() - toIndex.column();

        if ((qAbs(rowDelta) < 2) && (qAbs(columnDelta) < 2))
        {
            if (fromIndex.data().toInt() == toIndex.data().toInt())
            {
                emit clearSelection();
                return;
            }

            swapCells(fromIndex, toIndex);

            checkScene(toIndex, fromIndex, true);

            emit clearSelection();

        }
    }
}

void GameController::swapCells(const QModelIndex &first, const QModelIndex &second)
{
    int firstData   = first.data().toInt();
    int secondData  = second.data().toInt();

    std::swap(firstData, secondData);

    m_model->setData(first, firstData);
    m_model->setData(second, secondData);
}

void GameController::checkScene(const QModelIndex& from, const QModelIndex& to, bool clicked)
{
    // Сканируем все поле, на нахождение трех или более фишек
    // Начинаем счетчики с 1, так как подсчет ведется из количества шариков
    // То есть совпадение одно, а шариков то уже два
    ColumnToDelete  columnToDelete;
    RowToDelete     rowToDelete;

    for (int i = 0; i < m_model->columnCount(); ++i)
    {
        int rowMatches = 1;
        int columnMatches = 1;
        int startRowPoint       = -1;
        int startColumnPoint    = -1;
        for (int j = 0; j < m_model->columnCount(); ++j)
        {
            QModelIndex currentRowIndex = m_model->index(j, i);
            QModelIndex nextRowIndex = m_model->index(j + 1, i);

            int currRowType = currentRowIndex.data().toInt();
            int nextRowType = nextRowIndex.data().toInt();


            QModelIndex currentColumnIndex = m_model->index(i, j);
            QModelIndex nextColumnIndex = m_model->index(i, j + 1);

            int currColumnType = currentColumnIndex.data().toInt();
            int nextColumnType = nextColumnIndex.data().toInt();

            // Нашли что следующий шарик такой же, пометили колонку с которой это начинается
            // и прибавили число шариков, которые совпали
            if (nextRowIndex.isValid())
            {
                if (currentRowIndex.data().toInt() == nextRowIndex.data().toInt())
                {
                    if (rowMatches == 1) startRowPoint = j;
                    ++rowMatches;
                }
                else if (rowMatches < 3)
                {
                    rowMatches = 1;
                    startRowPoint = -1;
                }
                else if (rowMatches >= 3)
                {
                    rowToDelete.append({i, {startRowPoint, j}});
                    rowMatches = 1;
                    startRowPoint = -1;
                }
            }
            else if (rowMatches >=3 && (startRowPoint != -1))
            {
                rowToDelete.append({i, {startRowPoint, j}});
            }


            if (nextColumnIndex.isValid())
            {
                if (currentColumnIndex.data().toInt() == nextColumnIndex.data().toInt())
                {
                    if (columnMatches == 1) startColumnPoint = j;
                    ++columnMatches;
                }
                else if (columnMatches < 3)
                {
                    columnMatches = 1;
                    startColumnPoint = -1;
                }
                else if (columnMatches >= 3)
                {
                    columnToDelete.append({i, {startColumnPoint, j}});
                    columnMatches = 1;
                    startColumnPoint = -1;
                }
            }
            else if (columnMatches >=3 && (startColumnPoint != -1))
            {
                columnToDelete.append({i, {startColumnPoint, j}});
            }
        }
    }

    if (clicked)
    {
        if (rowToDelete.isEmpty() && columnToDelete.isEmpty())
        {
            QTimer::singleShot(REVERSE_SWAP_DELAY, [this, from, to]
            {
                swapCells(from, to);
            });
        }
    }
}





