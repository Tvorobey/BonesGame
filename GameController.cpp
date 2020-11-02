#include "StoneModel.h"
#include "GameController.h"
#include "Global.h"

#include <QDebug>
#include <QRandomGenerator>
#include <QTimer>

using namespace GlobalDefines;

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
    QModelIndex selectedItem;
    if (!selected.indexes().isEmpty()) selectedItem = selected.indexes().first();

    QModelIndex targetItem;
    if (!deselected.indexes().isEmpty()) targetItem   = deselected.indexes().first();

    // Проверяем был ли убран фокус с другой ячейки
    if (targetItem.isValid() && selectedItem.isValid())
    {
        // Проверим, что человеком была выбрана соседняя ячейка по вертикали или горизонтали
        int rowDelta    = selectedItem.row() - targetItem.row();
        int columnDelta = selectedItem.column() - targetItem.column();

        if ((qAbs(rowDelta) < 2) && (qAbs(columnDelta) < 2))
        {
            if (selectedItem.data().toInt() == targetItem.data().toInt())
            {
                emit clearSelection();
                return;
            }

            swapCells(selectedItem, targetItem);
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


