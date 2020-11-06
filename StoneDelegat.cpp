//Local
#include "StoneDelegat.h"
#include "Global.h"

//Qt
#include <QPainter>
#include <QStyledItemDelegate>
#include <QDebug>
#include <QApplication>

using namespace GlobalDefines;


StoneDelegat::StoneDelegat(QObject* parent)
    : QStyledItemDelegate(parent)
{

}

void StoneDelegat::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.data().isValid())
    {
        bool isOk = false;
        index.data().toInt(&isOk);

        if (!isOk)  QStyledItemDelegate::paint(painter, option, index);
        else
        {
            StoneColor type = static_cast<StoneColor>(index.data().toInt());

            double radius = option.rect.height()/2;
            double alignX = option.rect.width()/2 - radius;

            painter->save();

            painter->translate(option.rect.x() + alignX, option.rect.y());

            switch (type)
            {
                case Red: painter->setBrush({Qt::red}); break;
                case Blue: painter->setBrush({Qt::blue}); break;
                case Green: painter->setBrush({Qt::green}); break;
                case Yellow: painter->setBrush({Qt::yellow}); break;
                case Magenta: painter->setBrush({Qt::magenta}); break;
                default: break;
            }

            painter->drawEllipse(0, 0, radius*2, radius*2);

            painter->restore();
        }
    }
}


