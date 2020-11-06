//Local
#include "MainHandler.h"

#include <QDebug>

MainHandler::MainHandler(QObject *parent)
    : QObject(parent),
      m_mainWindow(new MainWindow()),
      m_gameController(new GameController(this)),
      m_stoneModel(new QStandardItemModel(this))
{
    m_mainWindow->show();

    m_mainWindow->setModel(m_stoneModel);
    m_gameController->setModel(m_stoneModel);

    connect(m_mainWindow.data(), &MainWindow::startGame, [this](int cellCount)
    {
        qDebug() << "enter";
        m_gameController->onStartGame(cellCount);
    });

    connect(m_mainWindow.data(), &MainWindow::stoneSelected,
            m_gameController.data(), &GameController::onStoneSelected);

    connect(m_gameController.data(), &GameController::clearSelection,
            m_mainWindow.data(), &MainWindow::clearSelections);

    connect(m_gameController.data(), &GameController::updateScore,
            m_mainWindow.data(), &MainWindow::updateScore);

    connect(m_gameController.data(), &GameController::loose,
            m_mainWindow.data(), &MainWindow::onLooseGame);
}
