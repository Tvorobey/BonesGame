#pragma once

#include <QStyledItemDelegate>

/*!
 * \brief The StoneDelegat - делегат для отрисовки наших шариков
 */
class StoneDelegat : public QStyledItemDelegate
{
public:
    StoneDelegat(QObject* parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};


