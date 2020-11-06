#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "StoneDelegat.h"

//Qt
#include <QDebug>
#include <QMessageBox>

namespace
{
    const int GAME_SCENE_INDEX      = 0;
    const int HELLOW_WINDOW_INDEX   = 1;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    prepareUi();
    createConnection();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setModel(QSharedPointer<QStandardItemModel> model)
{
    m_stoneModel = model;
    ui->gameSceneView->setModel(m_stoneModel.data());

    m_selectionModel = ui->gameSceneView->selectionModel();

    connect(m_selectionModel, &QItemSelectionModel::selectionChanged, [this](const QItemSelection &selected, const QItemSelection &deselected)
    {
        emit stoneSelected(selected, deselected);
    });
}

void MainWindow::clearSelections()
{
    m_selectionModel->clearSelection();
}

void MainWindow::updateScore(int score)
{
    ui->scoreLabel->setText(QString("Лучший счет: %1").arg(score));
}

void MainWindow::onLooseGame()
{
    QMessageBox msgBox;
    msgBox.setText("Вы проиграли.");
    msgBox.setInformativeText("Хотите сыграть еще?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setButtonText(QMessageBox::Ok, "Да!");
    msgBox.setButtonText(QMessageBox::Cancel, "Нет, усталь");

    msgBox.show();

    switch (msgBox.exec())
    {
        case QMessageBox::Ok:
        {
            ui->stackedWidget->setCurrentIndex(HELLOW_WINDOW_INDEX);
        }
        break;
        case QMessageBox::Cancel:
        {
            QApplication::quit();
        }
        break;
    }
}

void MainWindow::createConnection()
{
    connect(ui->btnStartGame, &QPushButton::clicked, [this]()
    {
        emit startGame(ui->spinBoxCellsCount->text().toInt());
        ui->stackedWidget->setCurrentIndex(GAME_SCENE_INDEX);
    });
}

void MainWindow::prepareUi()
{
    ui->scoreLabel->setText("Лучший счет: 0");
    ui->gameSceneView->setItemDelegate(new StoneDelegat());

    ui->spinBoxCellsCount->setRange(4, 100);
}

