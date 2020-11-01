#ifndef STONEDELEGAT_H
#define STONEDELEGAT_H

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

#endif // STONEDELEGAT_H
