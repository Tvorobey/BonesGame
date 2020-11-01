#ifndef MAINHANDLER_H
#define MAINHANDLER_H

//Qt
#include <QObject>
#include <QScopedPointer>
#include <QStandardItemModel>

//Local
#include "mainwindow.h"
#include "GameController.h"

class GameController;

class MainHandler : public QObject
{
    Q_OBJECT
public:
    explicit MainHandler(QObject *parent = nullptr);
signals:
private:
    QScopedPointer<MainWindow>          m_mainWindow;
    QScopedPointer<GameController>      m_gameController;
    QSharedPointer<QStandardItemModel>  m_stoneModel;
};

#endif // MAINHANDLER_H
