#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "StoneDelegat.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      popup(new HellowPopUp(this))
{
    createConnection();

    ui->setupUi(this);
    ui->ScoreLable->setText("Лучший счет: 0");
    ui->gameSceneView->setItemDelegate(new StoneDelegat());

    ui->gridLayout->addWidget(popup, 0, 0, 0, Qt::AlignCenter);

    popup->show();
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
    ui->ScoreLable->setText(QString("Лучший счет: %1").arg(score));
}

void MainWindow::createConnection()
{
    connect(popup, &HellowPopUp::startGame, [this](int cellCount)
    {
        popup->hide();

        emit startGame(cellCount);
    });
}

