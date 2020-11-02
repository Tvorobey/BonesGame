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
        }
    }
}

void GameController::onStoneSelected(const QItemSelection &selected, const QItemSelection &deselected)
{
    m_selectedStone     = selected.indexes();
    m_deselectedStone   = deselected.indexes();

    // Проверяем был ли убран фокус с другой ячейки
    if (!m_deselectedStone.isEmpty() && !m_selectedStone.isEmpty())
    {
        // Проверим, что человеком была выбрана соседняя ячейка по вертикали или горизонтали
        int rowDelta    = m_selectedStone.first().row() - m_deselectedStone.first().row();
        int columnDelta = m_selectedStone.first().column() - m_deselectedStone.first().column();

        if ((qAbs(rowDelta) < 2) && (qAbs(columnDelta) < 2))
        {
            if (m_selectedStone.first().data().toInt() == m_deselectedStone.first().data().toInt())
            {
                emit clearSelection();
                return;
            }

            swapCells(m_selectedStone.first(), m_deselectedStone.first());

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


