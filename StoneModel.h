#ifndef STONEMODEL_H
#define STONEMODEL_H

#include <QStandardItemModel>

class StoneModel : public QStandardItemModel
{
public:
    explicit StoneModel(QObject* parent = nullptr);
};

#endif // STONEMODEL_H
