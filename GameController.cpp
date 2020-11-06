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

GameController::GameController(QObject *parent)
    : QObject(parent),
      m_score(0)
{}

void GameController::setModel(QSharedPointer<QStandardItemModel> model)
{
    m_model = model;
}

void GameController::onStartGame(int cellCount)
{
    m_score = 0;

    m_model->setRowCount(cellCount);
    m_model->setColumnCount(cellCount);

    for (int i = 0; i < m_model->rowCount(); ++i)
    {
        for (int j = 0; j < m_model->columnCount(); ++j)
        {
            if ((i < 2) && (j < 2))
            {
                int type = QRandomGenerator::securelySeeded().bounded(StoneColor::ColorCount);
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
                       int type = QRandomGenerator::securelySeeded().bounded(StoneColor::ColorCount);
                       if (type != prevType)
                       {
                            m_model->setData(m_model->index(i, j), type, Qt::DisplayRole);
                            break;
                       }
                    }
                }
                else
                {
                    int type = QRandomGenerator::securelySeeded().bounded(StoneColor::ColorCount);
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
                       int type = QRandomGenerator::securelySeeded().bounded(StoneColor::ColorCount);

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
                    int type = QRandomGenerator::securelySeeded().bounded(StoneColor::ColorCount);
                    m_model->setData(m_model->index(i, j), type, Qt::DisplayRole);
                }
            }
        }
    }

    if (!victoryCheck())
    {
        qDebug() << "Сгенерировали так, что нет доступных комбинаций";
        onStartGame(m_model->rowCount());
    }
    else
        return;
}

void GameController::onStoneSelected(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndex fromIndex;
    if (!deselected.indexes().isEmpty()) fromIndex = deselected.indexes().first();

    QModelIndex toIndex;
    if (!selected.indexes().isEmpty()) toIndex   = selected.indexes().first();

    bool fromTypeOk = false;
    bool toTypeOk = false;
    fromIndex.data().toInt(&fromTypeOk);
    toIndex.data().toInt(&toTypeOk);

    // Проверяем был ли убран фокус с другой ячейки
    if (fromIndex.isValid() && toIndex.isValid() && fromTypeOk && toTypeOk)
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

            scanScene(true, toIndex, fromIndex);

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

void GameController::scanScene(bool clicked, const QModelIndex& from, const QModelIndex& to)
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

            bool isRowConvetOk = false;
            currentRowIndex.data().toInt(&isRowConvetOk);


            QModelIndex currentColumnIndex = m_model->index(i, j);
            QModelIndex nextColumnIndex = m_model->index(i, j + 1);

            bool isColumnConvertOk;
            currentColumnIndex.data().toInt(&isColumnConvertOk);

            // Нашли что следующий шарик такой же, пометили колонку с которой это начинается
            // и прибавили число шариков, которые совпали
            if (nextRowIndex.isValid() && isRowConvetOk)
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


            if (nextColumnIndex.isValid() & isColumnConvertOk)
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

    // Нажатие пользователя
    if (clicked)
    {
        if (rowToDelete.isEmpty() && columnToDelete.isEmpty())
        {
            QTimer::singleShot(REVERSE_SWAP_DELAY, [this, from, to]
            {
                swapCells(from, to);
            });
        }
        else
        {
            deleteMatches(columnToDelete, rowToDelete);
        }
    }
    // Очистка совпадений после того, как заполнили поле
    else
    {
        if (columnToDelete.isEmpty() && rowToDelete.isEmpty())
        {
            if (!victoryCheck())
            {
                qDebug() << "Looze";
                emit loose();
                return;
            }
            else
            {
                qDebug() << "Yeah go on!";
                return;
            }
        }

        deleteMatches(columnToDelete, rowToDelete);
    }
}

void GameController::deleteMatches(const ColumnToDelete &columnToDelete, const RowToDelete &rowToDelete)
{
    // Удалим шарики из колонок
    for (const auto& columnArray : columnToDelete)
    {
        int start   = columnArray.second.first;
        int end     = columnArray.second.second;

        m_score += (end + 1) - start;

        for (int i = start; i <= end; ++i)
        {
            QModelIndex index = m_model->index(columnArray.first, i);

            m_model->setData(index, "");
        }
    }

    // Удалим шарики из строк
    for (const auto& rowArray : rowToDelete)
    {
        int start   = rowArray.second.first;
        int end     = rowArray.second.second;

        m_score += (end + 1) - start;

        for (int i = start; i <= end; ++i)
        {
            QModelIndex index = m_model->index(i, rowArray.first);

            m_model->setData(index, "");
        }
    }

    emit updateScore(m_score);

    // Дропнем все шарики вниз
    shuffleDown();
}

void GameController::shuffleDown()
{
    // Падение шариков вниз

    for (int column = 0; column < m_model->columnCount(); ++column)
    {
        int fallDistance = 0;

        for (int row = m_model->rowCount() - 1; row >=0; --row)
        {
            QModelIndex index = m_model->index(row, column);

            bool isOk   = false;
            index.data().toInt(&isOk);

            if (!isOk) ++fallDistance;
            else
            {
                if (fallDistance > 0)
                {
                    int type = index.data().toInt();

                    m_model->setData(m_model->index(row + fallDistance, column), type);
                    m_model->setData(index, "");
                }
            }
        }
    }

    QTimer::singleShot(500, [this]()
    {
       floodFill();
    });
}

void GameController::floodFill()
{
    for (int column = 0; column < m_model->columnCount(); ++column)
    {
        for (int row = 0; row < m_model->rowCount(); ++row)
        {
            QModelIndex index = m_model->index(row, column);

            bool isOk = false;
            index.data().toInt(&isOk);

            // Если в первой строке, есть шарик, значит и дальше есть шарики
            if (isOk)  continue;
            else
            {
                int type = QRandomGenerator::securelySeeded().bounded(StoneColor::ColorCount);

                m_model->setData(index, type);
            }
        }
    }

    scanScene(false);
}

//NOTE: тут решение, вроде неплохое, но вот реализация непричесанная, в лоб так сказать
bool GameController::victoryCheck()
{
    qDebug() << "======================================";

    // NOTE: когда мы заходим в жту функцию, поле уже очищено от последовательностей шариков
    // от трех и более
    bool result = false;

    // Пробежимся по строчкам и столбцам
    for (int row = 0; row < m_model->rowCount(); ++row)
    {
        qDebug() << "Row: " << row;

        for (int column = 0; column < m_model->columnCount(); ++column)
        {
            qDebug() << "Column: " << column;

            int rowType = m_model->index(column, row).data().toInt();
            int columnType = m_model->index(row, column).data().toInt();

            bool isOk = false;

            m_model->index(row, column + 1).data().toInt(&isOk);

            // Два подряд идущих шара в строке
            if (isOk)
            {
                if (columnType == m_model->index(row, column + 1).data().toInt())
                {
                    qDebug() << "Два шарика подряд в строке"<< row << " " << column;

                    qDebug() << QString("Шарик в текущей ячейке %1, шарик в подходящей: %2")
                                .arg(columnType).arg(m_model->index(row, column + 1).data().toInt());

                    QModelIndex leftColumnIndex = m_model->index(row, column - 2);
                    QModelIndex leftTopColumnIndex    = m_model->index(row - 1, column -1);
                    QModelIndex leftBottomColumnIndex = m_model->index(row + 1, column -1);

                    QModelIndex rightColumnIndex = m_model->index(row, column + 3);
                    QModelIndex rightTopColumnIndex = m_model->index(row - 1, column + 2);
                    QModelIndex rightBottomColumnIndex = m_model->index(row + 1, column + 2);

                    if (leftColumnIndex.isValid() && (leftColumnIndex.data().toInt() == columnType))
                    {
                        qDebug() << QString("leftColumnIndex row = %1, column = %2")
                                    .arg(leftColumnIndex.row()).arg(leftColumnIndex.column());

                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(leftColumnIndex.data().toInt());
                        result = true;
                        return result;
                    }
                    if (leftTopColumnIndex.isValid() && (leftTopColumnIndex.data().toInt() == columnType))
                    {
                        qDebug() << QString("leftTopColumnIndex row = %1, column = %2")
                                    .arg(leftTopColumnIndex.row()).arg(leftTopColumnIndex.column());

                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(leftTopColumnIndex.data().toInt());

                        result = true;
                        return result;
                    }
                    if (leftBottomColumnIndex.isValid() && (leftBottomColumnIndex.data().toInt() == columnType))
                    {
                        qDebug() << QString("leftBottomColumnIndex row = %1, column = %2")
                                    .arg(leftBottomColumnIndex.row()).arg(leftBottomColumnIndex.column());
                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(leftBottomColumnIndex.data().toInt());

                        result = true;
                        return result;
                    }

                    if (rightColumnIndex.isValid() && (rightColumnIndex.data().toInt() == columnType))
                    {
                        qDebug() << QString("rightColumnIndex row = %1, column = %2")
                                    .arg(rightColumnIndex.row()).arg(rightColumnIndex.column());
                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(rightColumnIndex.data().toInt());

                        result = true;
                        return result;
                    }
                    if (rightTopColumnIndex.isValid() && (rightTopColumnIndex.data().toInt() == columnType))
                    {
                        qDebug() << QString("rightTopColumnIndex row = %1, column = %2")
                                    .arg(rightTopColumnIndex.row()).arg(rightTopColumnIndex.column());
                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(rightTopColumnIndex.data().toInt());

                        result = true;
                        return result;
                    }
                    if (rightBottomColumnIndex.isValid() && (rightBottomColumnIndex.data().toInt() == columnType))
                    {
                        qDebug() << QString("rightBottomColumnIndex row = %1, column = %2")
                                    .arg(rightBottomColumnIndex.row()).arg(rightBottomColumnIndex.column());
                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(rightBottomColumnIndex.data().toInt());

                        result = true;
                        return result;
                    }
                }
            }

            m_model->index(row, column + 2).data().toInt(&isOk);

            if (isOk)
            {
                // Два шарика, но которые идут через один в строке
                if (columnType == m_model->index(row, column + 2).data().toInt())
                {
                    qDebug() << "Два шарика через один в строке"<< row << " " << column;

                    qDebug() << QString("Шарик в текущей ячейке %1, шарик в подходящей: %2")
                                .arg(columnType).arg(m_model->index(row, column + 2).data().toInt());

                    QModelIndex topRowIndex = m_model->index(row - 1, column + 1);
                    QModelIndex bottomRowIndex = m_model->index(row + 1, column + 1);

                    if (topRowIndex.isValid() && (topRowIndex.data().toInt() == columnType))
                    {
                        qDebug() << QString("topRowIndex row = %1, column = %2")
                                    .arg(topRowIndex.row()).arg(topRowIndex.column());
                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(topRowIndex.data().toInt());

                        result = true;
                        return result;
                    }
                    if (bottomRowIndex.isValid() && (bottomRowIndex.data().toInt() == columnType))
                    {
                        qDebug() << QString("bottomRowIndex row = %1, column = %2")
                                    .arg(bottomRowIndex.row()).arg(bottomRowIndex.column());
                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(bottomRowIndex.data().toInt());

                        result = true;
                        return result;
                    }
                }
            }

            m_model->index(column + 1, row).data().toInt(&isOk);

            if (isOk)
            {
                // Два шарика стоят подряд в колонке
                if (rowType == m_model->index(column + 1, row).data().toInt())
                {
                    qDebug() << "Два шарика подряд в колонке"<< column << " " << row;

                    qDebug() << QString("Шарик в текущей ячейке %1, шарик в подходящей: %2")
                                .arg(columnType).arg(m_model->index(column + 1, row).data().toInt());

                    QModelIndex topRowIndex = m_model->index(column - 2, row);
                    QModelIndex topLeftRowIndex = m_model->index(column - 1, row - 1);
                    QModelIndex topRightRowIndex = m_model->index(column - 1, row + 1);

                    QModelIndex bottomRowIndex = m_model->index(column + 3, row);
                    QModelIndex bottomLeftRowIndex = m_model->index(column + 2, row -1);
                    QModelIndex bottomRightIndex = m_model->index(column + 2, row +1);

                    if (topRowIndex.isValid() && (topRowIndex.data().toInt() == rowType))
                    {
                        qDebug() << QString("topRowIndex row = %1, column = %2")
                                    .arg(topRowIndex.column()).arg(topRowIndex.row());
                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(topRowIndex.data().toInt());

                        result = true;
                        return result;
                    }
                    if (topLeftRowIndex.isValid() && (topLeftRowIndex.data().toInt() == rowType))
                    {
                        qDebug() << QString("topLeftRowIndex row = %1, column = %2")
                                    .arg(topLeftRowIndex.column()).arg(topLeftRowIndex.row());
                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(topLeftRowIndex.data().toInt());

                        result = true;
                        return result;
                    }
                    if (topRightRowIndex.isValid() && (topRightRowIndex.data().toInt() == rowType))
                    {
                        qDebug() << QString("topRightRowIndex row = %1, column = %2")
                                    .arg(topRightRowIndex.column()).arg(topRightRowIndex.row());
                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(topRightRowIndex.data().toInt());

                        result = true;
                        return result;
                    }

                    if (bottomRowIndex.isValid() && (bottomRowIndex.data().toInt() == rowType))
                    {
                        qDebug() << QString("bottomRowIndex row = %1, column = %2")
                                    .arg(bottomRowIndex.column()).arg(bottomRowIndex.row());
                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(bottomRowIndex.data().toInt());

                        result = true;
                        return result;
                    }
                    if (bottomLeftRowIndex.isValid() && (bottomLeftRowIndex.data().toInt() == rowType))
                    {
                        qDebug() << QString("bottomLeftRowIndex row = %1, column = %2")
                                    .arg(bottomLeftRowIndex.column()).arg(bottomLeftRowIndex.row());
                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(bottomLeftRowIndex.data().toInt());

                        result = true;
                        return result;
                    }
                    if (bottomRightIndex.isValid() && (bottomRightIndex.data().toInt() == rowType))
                    {
                        qDebug() << QString("bottomRightIndex row = %1, column = %2")
                                    .arg(bottomRightIndex.column()).arg(bottomRightIndex.row());
                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(bottomRightIndex.data().toInt());

                        result = true;
                        return result;
                    }
                }
            }

            m_model->index(column, row + 2).data().toInt(&isOk);

            if (isOk)
            {
                // В колонке есть два одинаковых шарика, которые стоят через один
                if (rowType == m_model->index(column, row + 2).data().toInt())
                {
                    qDebug() << "Два шарика через один в колонке" << column << " " << row;

                    qDebug() << QString("Шарик в текущей ячейке %1, шарик в подходящей: %2")
                                .arg(columnType).arg(m_model->index(column, row + 2).data().toInt());

                    QModelIndex leftColumnIndex = m_model->index(column + 1, row - 1);
                    QModelIndex rightColumnIndex = m_model->index(column + 1, row + 1);

                    if (leftColumnIndex.isValid() && (leftColumnIndex.data().toInt() == rowType))
                    {
                        qDebug() << QString("leftColumnIndex row = %1, column = %2")
                                    .arg(leftColumnIndex.column()).arg(leftColumnIndex.row());
                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(leftColumnIndex.data().toInt());

                        result = true;
                        return result;
                    }
                    if (rightColumnIndex.isValid() && (rightColumnIndex.data().toInt() == rowType))
                    {
                        qDebug() << QString("rightColumnIndex row = %1, column = %2")
                                    .arg(rightColumnIndex.column()).arg(rightColumnIndex.row());
                        qDebug() << QString("Тип первого шарика %1, тип подходящего шарика %2")
                                    .arg(columnType).arg(rightColumnIndex.data().toInt());

                        result = true;
                        return true;
                    }
                }
            }
        }
    }

    return result;
}





