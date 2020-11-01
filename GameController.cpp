#include "StoneModel.h"
#include "GameController.h"
#include "Global.h"

#include <QDebug>
#include <QRandomGenerator>

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
            quint64 type = QRandomGenerator::global()->bounded(StoneColor::ColorCount);

            m_model->setData(m_model->index(i, j), type, Qt::DisplayRole);
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
        int rowDelta    = qAbs(m_selectedStone.first().row() - m_deselectedStone.first().row());
        int columnDelta = qAbs(m_selectedStone.first().column() - m_deselectedStone.first().column());

        if ((rowDelta < 2) && (columnDelta < 2))
        {
            // Запомнили значение в ячейке, которую хотим поменять местами с другой
            int tmpType = m_selectedStone.first().data().toInt();

            m_model->setData(m_selectedStone.first(), m_deselectedStone.first().data());
            m_model->setData(m_deselectedStone.first(), tmpType);

            emit clearSelection();
        }
    }
}
